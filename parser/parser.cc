#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>
#include"../common/util.hpp"

const std::string input_path = "../data/input/html";
const std::string output_path = "../data/tmp/raw_input";


//doc文件是带搜索的html
struct DocInfo
{
  std::string title;
  std::string url;
  std::string content;
};


bool ParseTitle(const std::string& html,std::string* title)
{
  
     size_t pos = html.find("<title>");
     if(pos == std::string::npos)
     {
     std::cout<<"ont found title"<<std::endl;
     return false;
     }
     size_t end = html.rfind("</title>");
     if(end == std::string::npos)
     {
     std::cout<<"ont found title"<<std::endl;
        return false;
     }
     size_t beg = pos + std::string("<title>").size();
   *title += html.substr(beg,end - beg);
  
/*
  size_t beg = html.find(">");
  if(beg == std::string::npos)
  {
    std::cout<<"ont found title"<<std::endl;
    return false;
  }
  size_t end = html.find("<");
  if(end == std::string::npos)
  {
    std::cout<<"ont found title"<<std::endl;
    return false;
  }

  if(beg < end)
  {
    *title = html.substr(beg,end);
  }
  */
  return true;
}


//获取正文，去标签，但是会将标签也会读到里边
bool ParseContent(std::string& html,std::string* content)
{
  bool is_content = true;
  for(auto& e : html)
  {
    if(is_content)
    {
      if(e == '<')
        is_content = false;
      else
      {
        //为了后边的行文本文件
        if(e =='\n')
        {
          e = ' ';
        }
        content->push_back(e);
      }
    }
    else{
      if(e == '>')
        is_content = true;
    }
  }
  return true;
}

//获取url
bool ParseUrl(const std::string& file_path ,std::string* url)
{
  //https://www.boost.org/doc/libs/1_59_0/
  size_t sz = input_path.size();
  std::string temp;
  temp += file_path.substr(sz);
  std::string ret = "https://www.boost.org/doc/libs/1_59_0/doc";
  *url += ret + temp;
  return true;
}


//将文件枚举，去除掉其中的不是html的路径
bool EnumFile(const std::string& input_path,std::vector<std::string>* file_list)
{
  namespace fs = boost::filesystem;
  //input_path 是一个字符串，根据这个字符串构造出这个path
  fs::path root_path(input_path);
  if(!(fs::exists(root_path)))
  {
    std::cout<<"input_path not exists input_path = "<<input_path<<std::endl;
    return false;
  }

  // fs::recursive_directory_iterator end_iter;
  fs::directory_iterator end_iter;
  //boost递归遍历boost目录
  for(fs::directory_iterator iter(root_path);iter != end_iter;++iter){
    //去除目录
    /*
    if(fs::is_regular_file(*iter))
    {
      continue;
    }
    */
    //去除非.html文件
    if(iter->path().extension() != ".html")
    {
      continue;
    }
    file_list->push_back(iter->path().string());
  }
  return true;
}


//获取文件的内容，解析url,正文，标题，去标签
bool Parsefile(const std::string& file_path,DocInfo* info)
{
  //打开文件，读取内容
  std::string html;
  bool ret =  FileUtil::Read(file_path,&html);
  if(!ret)
  {
    std::cout<<"Read false  file_path = "<<file_path<<std::endl;
    return false;
  }
  //解析标题
  ret = ParseTitle(html,&info->title);
  if(!ret)
  {
    std::cout<<"ParseTitle false  file_path = "<<file_path<<std::endl;
    return false;  
  }
  //获取正文，url,去标签
  ret = ParseContent(html,&info->content);
  if(!ret)
  {
    std::cout<<"ParseContent false  file_path = "<<file_path<<std::endl;
    return false;
  }
  //url
  ret = ParseUrl(file_path,&info->url);
  if(!ret)
  {
    std::cout<<"ParseUrl false  file_path = "<<file_path<<std::endl;
    return false;
  }
  return true;
}

//将html输入到行文本文件，每一行就是一个html,利用特殊符号将url,正文，标题分开
void WriteOutPut(const DocInfo& info,std::ofstream& file)
{
    std::string line = info.title + '\3' + info.url + '\3' + info.content + '\n';
    file.write(line.c_str(),line.size());
}



int main()
{
  //1.枚举出所有路径中html文档的路径,将每个文件路径保存在vector中
  std::vector<std::string> file_list;
  bool ret = EnumFile(input_path,&file_list);
  if(!ret)
  {
    std::cout<<"EnumFile failed"<<std::endl;
    return 1;
  }

/*  
     for(size_t i = 0;i < file_list.size();++i)
     {

     std::cout<<file_list[i]<<std::endl;
     }
*/     

  //路径：home/linux/33/SearchEngines/data/input
  //2。循环遍历vector将每个路径中的的/标题/正文/url分析出来

  std:: ofstream output_file(output_path.c_str());
  if(!output_file)
  {
    std::cout<<"ofstream output_file failed"<<std::endl;
  }

  for(auto& file_path : file_list)
  {
    DocInfo info;

    ret = Parsefile(file_path,&info);
    if(!ret)
    {
      std::cout<<"Parsefile failed   file_path = "<<file_path<<std::endl;
      continue;
    }

    WriteOutPut(info,output_file);
  }
  output_file.close();

  return 0;
}




