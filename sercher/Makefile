FLAG =-std=c++11 -lboost_filesystem -lboost_system \
	-I /home/linux/33/cppjieba/include -ljsoncpp 

.PHONY:all 
all: test libsearcher.so

test:test.cc searcher.h searcher.cc 
	g++ test.cc searcher.cc -g -o test $(FLAG)


libsearcher.so:searcher.cc searcher.h 
	g++ searcher.cc -shared -fPIC -o libsearcher.so $(FLAG)

http_server:http_server.cpp libsearcher.so                                                              
	g++ http_server.cpp libsearcher.so -o http_sever -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f http_server test
 
