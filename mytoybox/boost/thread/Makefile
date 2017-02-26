CXX=/home/onega/gcc-6.3.0/bin/g++
LDFLAGS=-Wl,-rpath,'$$ORIGIN' -Wl,-rpath=/home/onega/gcc-6.3.0/lib64 -lboost_thread -lboost_system
LDFLAGS=-Wl,-rpath=/home/onega/gcc-6.3.0/lib64 -lboost_thread -lboost_system
LDFLAGS=-Wl,-rpath,/home/onega/gcc-6.3.0/lib64 -lboost_thread -lboost_system
CXXFLAGS= -fno-omit-frame-pointer -fsanitize=address -std=c++11 
CXXFLAGS= -fno-omit-frame-pointer -std=c++14 -pthread -fsanitize=thread -fPIE 
# gcc-6.3.0 does not support -fsanitize=memory -fsanitize-blacklist
#CXXFLAGS= -fsanitize=memory -fPIE -pie  -std=c++14
#-fsanitize-blacklist=tsan.blacklist
#CXXFLAGS= -fno-omit-frame-pointer -std=c++14 -pthread 

all: lockfreetest

lockfreetest: thread.cpp
	$(CXX) -g3 $(CXXFLAGS) $(LDFLAGS) $^ -o $@ 

clean: 
	@if [ -f lockfreetest ] ; then rm lockfreetest ; fi ;
