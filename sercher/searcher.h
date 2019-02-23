#pragma once 
#include<string>
#include<algorithm>
#include<unordered_map>
#include<boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
#include"jsoncpp/json/json.h"
#include<vector>
//构建正排索引和倒排索引
namespace sercher{ 

const char* const DICT_PATH = "..jiebadit/dict/jieba.dict.utf8";
const char* const HMM_PATH = "..jiebadit/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "..jiebadit/dict/user.dict.utf8";
const char* const IDF_PATH = "..jiebadit/dict/idf.utf8";
const char* const STOP_WORD_PATH = "..jiebadit/dict/stop_words.utf8";

  struct DocInfo{ 
    std::string title; 
    std::string content;
    std::string url;
    uint64_t doc_id;
  };
  
  struct Weight{ 
  uint64_t doc_id;
  int weight;
  std::string key;        
  };

  class Index {
   public:
     //构建索引
  Index():jieba_(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH){
  }
     bool Build(std::string input_path);
     //查正排
     DocInfo* GetDocInfo(uint64_t doc_id);
     //查倒排
     std::vector<Weight>* GetInvertedList(std::string key);

     void CutWord(std::string& input,std::vector<std::string>* output);
     
   private:
     //整排索引
     std::vector<DocInfo> forward_index;
     //倒排索引--->利用关键词找到相应的文件id
     std::unordered_map<std::string,std::vector<Weight>> inverted_index;
     cppjieba::Jieba jieba_;

     DocInfo* BuildForward(std::string& line);

     void BuildInverted(DocInfo& info);
     
  };
  void Index::CutWord(std::string& input,std::vector<std::string>* output){
    jieba_.CutForSearch(input,*output);
  }

  
  //搜索模块
  class sercher{
  public:
    sercher():index_(new Index){

    }

    ~sercher()
    {
      delete index_;
    }
    //加载索引
    bool Init(std::string& input_path);
    //通过特定的格式在resul字符串中表示搜索结果
    bool Search(std::string& query,std::string& result);
  private:
    Index* index_;
    std::string GetDesc(const std::string& content,const std::string& key);
  };

  std::string sercher::GetDesc(const std::string& content,const std::string& key){
    //1.现在正文中查找该词的位置，然后向前往后查找一段
    size_t pos = content.find(key);
    if(pos == std::string::npos)
    {
       if(content.size() < 100)
         return content;
       else 
         return content.substr(0,100) + "...";
    }
    size_t beg = 0;
    if(pos < 50)
      beg = 0;
    else 
      beg = pos - 50;
    if(pos + 50 < content.size())
      return content.substr(beg);
    else 
      return content.substr(beg,pos + 50) + "...";
  }

    //加载索引
  bool sercher::Init(std::string& input_path){
      return index_->Build(input_path);

  }

    //通过特定的格式在resul字符串中表示搜索结果
    bool sercher::Search(std::string& query,std::string& json_result){
        //1.分词：对查询词进行分词
        std::vector<std::string> tokens;
        index_->CutWord(query,&tokens);
        //2.触发: 针对分词进行查找倒排索引，找到相关的文档
        //遍历查找--->忽略大小写
        std::vector<Weight> all_result;
        for(auto& word : tokens)
        {
          boost::to_lower(word);
          auto* inverted_list = index_->GetInvertedList(word);
          if(inverted_list == NULL)
          {
            continue;
          }
          //不同的分词对应相同文档，这时候就需要去重，合并权重   insert  是在指定区间内插入所有指定元素
          all_result.insert(all_result.end(),inverted_list->begin(),inverted_list->end());
        }
        //3.排序：将找出的文档按照权重weight排序
        //sort的第三个参数可以为lamda 
        std::sort(all_result.begin(),all_result.end(),[](const Weight& w1,const Weight& w2){
              return w1.weight > w2.weight;
            });
        //4.构造结果：查正排，找到每个搜素结果的正文中标题，url,正文
          Json::Value results;
        for(auto& weight : all_result)
        {
          auto* info = index_->GetDocInfo(weight.doc_id);
          if(info == NULL)
          {
            continue;
          }
          
          //使用JSON构造
          Json::Value result;
          result["title"] = info->title;
          result["url"] = info->url;
          result["content"] = GetDesc(info->content,weight.key);
          results.append(result);
        }
        Json::FastWriter writer;
        json_result = writer.write(results);
      return true;
    }
  
}//end of sercher
