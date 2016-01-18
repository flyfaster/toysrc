#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <fstream>
#include "ace/ARGV.h"
#include "ace/Argv_Type_Converter.h"
#include "ace/Get_Opt.h"
#include <sys/auxv.h>

class testcompatible
{
  virtual int version()=0;
  virtual void a()=0;
  virtual void a(int c)=0;

};

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

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
char exepath[MAXPATHLEN]; 

std::string get_process_name(int pid) {
	char exepath[MAXPATHLEN]={0}; 
	char pathbuf[128];
	sprintf(pathbuf, "/proc/%d/exe", pid);
	readlink(pathbuf, exepath, sizeof(exepath));
	return exepath;
}

	char* itoa(int value, char* result, int base) { // http://www.strudel.org.uk/itoa/
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}
  uint64_t getCurrentMillis() {
    struct timeval tv;
    struct timezone tz;
    int sys = gettimeofday(&tv, &tz);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  }	
int main (int argc, char* argv[])
{ 
  char parent_process_path_buf[128];
  sprintf(parent_process_path_buf, "/proc/%d/cmdline", getppid());
  std::ifstream parent_cmdline;
  parent_cmdline.open(parent_process_path_buf);
  std::string parent_cmdline_str;
  std::getline(parent_cmdline, parent_cmdline_str);  
  
  parent_cmdline.close();
  std::cout <<(char *)getauxval(AT_EXECFN) << " pid " << getpid() << " Parent PID is " << getppid() << " " <<parent_cmdline_str << " " <<currentDateTime() << std::endl;
  usleep(1000000* (rand()%10+1));
  std::cout <<get_process_name(getpid()) << " pid " << getpid() << " Parent PID is " << getppid() << " " <<parent_cmdline_str << " " <<currentDateTime() << std::endl;
  std::cout << "Resource size is " << (size_t)&_binary_main_cpp_size<<std::endl;
  std::string src(&_binary_main_cpp_start,(size_t)( &_binary_main_cpp_end-&_binary_main_cpp_start));
     ACE_Argv_Type_Converter to_tchar (argc, argv);
   ACE_Get_Opt get_opt (argc,
                        to_tchar.get_TCHAR_argv (),
                        ACE_TEXT ("msad:"), 1, 0);
   //ACE_Get_Opt get_opt (argc,argv, ACE_TEXT ("P"), 1, 0);
   for (int c; (c = get_opt ()) != -1; )
     switch (c)
       {
       case 's': // print source code
         std::cout <<src << std::endl;
         break;
       case 'm': // print free memory information
       {
		   std::cout << "Parent process is " << get_process_name(getppid()) << std::endl;
		     std::string ss=runcmd("free -m");
		std::cout << ss <<std::endl; 
		}   
         break;		
       case 'a': // test ACE_Argv_Type_Converter
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
		}   
         break;	   
       case 'd': // compare performance of itoa and sprintf
       {
		  int cnt = ACE_OS::atoi(get_opt.opt_arg ());
		  uint64_t perf1start= getCurrentMillis();
		  char buf[16];
		  for(int i=0; i<cnt; i++)
			itoa(i, buf, 10);
		  uint64_t perf2start= getCurrentMillis();
		  for(int i=0; i<cnt; i++)
			sprintf(buf, "%d", i);
		  uint64_t perf2stop= getCurrentMillis();
		  std::cout << "convert "<<cnt<<" int to string by itoa in " 
			<< (perf2start-perf1start) << " milliseconds, sprintf in " 
			<< (perf2stop-perf2start) << " milliseconds\n";
		}   
         break;	               
       default:
         break;
       }

  return 0;
}
/*
 * objcopy --input binary --output elf32-i386 --binary-architecture i386 main.cpp mydata.o
 * nm -C mydata.o
 * g++ -o main.o -c main.cpp -I/home/onega/3rdlib/ACE_wrappers
 * g++ -L/home/onega/3rdlib/ACE_wrappers/lib  -o testapp main.o mydata.o -lACE
objcopy --input binary --output elf32-i386 --binary-architecture i386 main.cpp test.o --redefine-sym _binary_main_cpp_end=mydata_end

 */

