#include"searcher.h"

int main()
{
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
#if 0
  sercher::sercher serch;
  bool ret = serch.Init("../data/tmp/raw_input");
  if(!ret)
  {
    return 1;
  }

  std::string query = "filesystem";
  std::String result;
  serch.Search(query,&result);
  std::cout<<"result: "<<result<<endl;
#endif
return 0;
}
