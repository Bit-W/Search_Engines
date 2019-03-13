#include"searcher.h"
#include<string>
#include<iostream>

int main()
{
#if 0
  sercher::Index index;
  bool ret = index.Build("../data/tmp/raw_input");
  if(!ret)
  {
    std::cout<<"Buile is false"<<std::endl;
    return 1;
  }
  auto* inverted_list = index.GetInvertedList("filesystem");
  if(inverted_list == NULL)
  {
    std::cout<<"GetInvertedList is faild"<<std::endl;
    return 1;
  }
  for(auto& weight : *inverted_list)
  {
    std::cout<<"id: "<<weight.doc_id<<"wight: "<<weight.weight<<std::endl;
    auto* info = index.GetDocInfo(weight.doc_id);
    std::cout<<"title: "<<info->title<<std::endl;
    std::cout<<"utl: "<<info->url<<std::endl;
  }
#endif 

  sercher::sercher serch;
  std::string ss = "../data/tmp/raw_input";
  bool ret = serch.Init(ss);
  if(!ret)
  {
    return 1;
  }

  std::string query = "filesystem";
  std::string result;
  serch.Search(query,result);
  std::cout<<"result: "<<result<<std::endl;
  return 0;

}
