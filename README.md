
基于boost库的搜索引擎  

实现的功能：  

当输入关键字后能够返回有关该关键字的html文档，并将这些个文档进行按照词频的排序  


实现项目大致分为三大模块实现：

1.构建文档：  
  1.）首先将boost库中的1.69版本的离线文档下载下来（这里可以自行选择文档版本）  
  
  2.）将离线文档中的html目录下的所有html文档都读取到一个文件中，构成行文本文件（这里利用filesystem这个第三方库函数将目录和不是html后缀的文档进行去除）  
  3.）当读取到一个文档中后，每一行就是一个html，利用‘\3’对标题，正文，url进行分隔，作为区分。（区分符号选择要必须是文中没有出现的）  
  
  4.）这时候构建文档模块就完成了  
  
2.构建倒排索引和正排索引（该项目的重点）  

  1.）正排索引：利用文档id找到相应文档的内容  
      构建过程：  
      1.首先将第一个模块中的行文本文件按行放到Docinfo这个架构体中。Docinfo结构体中的成员为，正文，url，标题     
      
      2.将构建好的Docinfo结构体尾插进一个vector中（vector<Docinfo>)  
      
      3.这时候vector的下标就是相应文档的id  
  
  2.）倒排索引：通过关键字找到相应文档的id   
      1.首先利用第三方库函数cppjieba将正文，标题进行分词。  
      
      2.将分词后的关键字进行词频统计（这里的词频统计我直接就是如果在标题中出现我会乘以10，在正文中出现我会乘以2），将词频防卫weigth这个结构体中  
      
      3.然后将该关键字和相应统计好的词频放入unordered_map中（因为该stl容器底层为哈希，所以插入的效率很高，查找的效率也很高）
    
3.搜索模块  

   1.）分词
       利用第三方库将要搜索的关键字进行分词  
       
   2.）查倒排：  
       将分词后的结果去查倒排索引找到相应的文档id，并对找到的文档id更具权重进行降序排序  
       
   3.）查正排
       将排序后的文档id去查找正排索引，找到相应的文档内容
   
   4.）构建返回
       将查找出来的文档内容进程标题 + url + 正文 构建返回。（这里构建返回的时候我是返回一部分内容，对正文和标题进行的截取）
       
