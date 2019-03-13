#include"searcher.h"
#include<iostream>
#include"../common/util.hpp"

namespace sercher{

  //查正排
  DocInfo* Index::GetDocInfo(uint64_t doc_id){
    if(doc_id >= forward_index.size()) //找到返回，没找到返回空
      return NULL;
    return &forward_index[doc_id];
  }

  //查倒排
  std::vector<Weight>* Index:: GetInvertedList(std::string key){
    //直接在underod_map中查找
    auto pos = inverted_index.find(key);
    if(pos == inverted_index.end())
    {
      std::cout<<"not found key"<<std::endl;
      return NULL;
    }
    return &pos->second;
  }

  //构建索引
  bool Index::Build(std::string input_path){
    std::cout<<"build index is start"<<std::endl;
    //1.按行读取文件内容，每一行就是一个html
    //打开一个文件，并且按行读取，但是并不包括结尾的\n
    std::ifstream file(input_path.c_str());
    if(!file.is_open())
    {
      std::cout<<"open the input_path false  input_path = "<<input_path<<std::endl;
      return false;
    }
    std::string line;
    while(std::getline(file,line)){  
      //2.构造DocInfo对象，更新正排索引
      DocInfo* info = BuildForward(line);
      //3.更新倒排索引数据
      BuildInverted(*info);
      if(info->doc_id % 500 == 0){
        std::cout<<"already build: "<<(info->doc_id)<<std::endl;
      }
    }
    file.close();
    std::cout<<"index build finish!"<<std::endl;  
    return true;
  }

    //更新正排索引
    DocInfo* Index::BuildForward(std::string& line){

      //对读取的一行进行切分，区分出url.正文，标题
      std::vector<std::string> tokens;    //存放切分结果
      StringUtil::Split(line,&tokens,"\3");
      if(tokens.size() != 3)
      {
        std::cout<<"tokens is false"<<std::endl;
        return NULL;
      }

      //构造为一个DocInfo对象
      DocInfo info;
      info.doc_id = forward_index.size();
      info.title = tokens[0];
      info.url = tokens[1];
      info.content = tokens[2];

      //把这个对象插入正排索引中
      forward_index.push_back(info);

      return &forward_index.back();

    }  

    //更新倒排索引
    void Index::BuildInverted(DocInfo& info){

      //1.先进行分词,标题正文都要分
      std::vector<std::string> title_tokens;//存放标题的分词结果
      CutWord(info.title,&title_tokens);

      std::vector<std::string> content_tokens;//存放正文的分词结果
      CutWord(info.content,&content_tokens);

      struct WordCut{  //定义一个结构体来统计正文和标题的词频
        int title_cnt;
        int content_cnt;
      };

      //2.在进行词频统计（用一个哈希表去统计）
      std::unordered_map<std::string,WordCut> word_cnt;

      for(auto& word : title_tokens){

        boost::to_lower(word); //全部变换为小写
        ++(word_cnt[word].title_cnt);
      }

      for(auto& word : content_tokens){
        boost::to_lower(word);   //忽略大小写
        ++(word_cnt[word].content_cnt);
      }

      //3.遍历分词结果，在倒排中查找
      for(auto& word_pair : word_cnt){
        Weight weight;
        weight.doc_id = info.doc_id;
        weight.weight = 20 * word_pair.second.title_cnt + word_pair.second.content_cnt; 
        weight.key = word_pair.first;
        //4.不存在就插入
      //5.存在的话就进行，找到对应的值，构建weight
      std:: vector<Weight>& inverted_list = inverted_index[word_pair.first];
      inverted_list.push_back(weight);

    }
      }
}//end of namespace 
