#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include "ace/ARGV.h"
#include "ace/Argv_Type_Converter.h"

class testcompatible
{
  virtual int version()=0;
  virtual void a()=0;
  virtual void a(int c)=0;

};

std::string runcmd(const std::string& cmd) {
    std::stringstream ss;
    FILE *in;
    char buff[1024];
    if(!(in = popen(cmd.c_str(), "r"))) {
        return "";
    }
    while(fgets(buff, sizeof(buff), in)!=NULL) {
        ss << buff;
        usleep(10);
    }
    pclose(in);
    return ss.str();
}
  extern char _binary_main_cpp_end, _binary_main_cpp_start, _binary_main_cpp_size;
int main ()
{
  ACE_ARGV_T<char> argslist;
  argslist.add("abc def ghi");
  argslist.add("4th");
  argslist.add("5th");
  int newargc=argslist.argc();
  ACE_Argv_Type_Converter cmdline(newargc, argslist.argv());
  char** newargs=cmdline.get_ASCII_argv();
  std::cout << "arg count " << argslist.argc()<<std::endl;
  for(int i=0; i<newargc;i++)
    std::cout<< "argv["<<i<<"] " << newargs[i]<<std::endl;
  std::string ss=runcmd("free -m");
  std::cout << ss <<std::endl;  
  
  std::cout << "Resource size is " << (int)&_binary_main_cpp_size<<std::endl;
  std::string src(&_binary_main_cpp_start,(size_t)( &_binary_main_cpp_end-&_binary_main_cpp_start));
  std::cout <<src << std::endl;
  return 0;
}
/*
 * objcopy --input binary --output elf32-i386 --binary-architecture i386 main.cpp mydata.o
 * nm -C mydata.o
 * g++ -o main.o -c main.cpp -I/home/onega/3rdlib/ACE_wrappers
 * g++ -L/home/onega/3rdlib/ACE_wrappers/lib  -o testapp main.o mydata.o -lACE
objcopy --input binary --output elf32-i386 --binary-architecture i386 main.cpp test.o --redefine-sym _binary_main_cpp_end=mydata_end

 */

