#include <iostream>
#include <string> 
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <deque>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
// #include "ace/ARGV.h"
// #include "ace/Argv_Type_Converter.h"
/*
 *  DUMP_SYMS=/usr/bin/dump_syms
 *  storesym symbol_root_folder  library_file
 *  ./dump_syms ./breakpadtest > breakpadtest.sym
 *  head -n1 breakpadtest.sym
 *  mkdir -p ./sym/breakpadtest/... 
 *  mv breakpadtest.sym sym/breakpadtest/... 
 * 
 */

std::string run_cmd(const char* cmdstr)
{
  std::stringstream ss;
  const int MAX_BUFFER = 2048;    
    char buffer[MAX_BUFFER];
    FILE *stream =popen(cmdstr, "r");// popen("grep MemFree /proc/meminfo", "r");
    if (stream){
       while (!feof(stream))
       {
            if (fgets(buffer, MAX_BUFFER, stream) != NULL)
            {              
	       		ss << buffer;
            }
	    usleep(1);
       }
       pclose(stream);
    }  
    return ss.str();
}
int main(int argc, char **argv) {
  if (argc < 3)    
  {
    std::cout << "usage: " <<argv[0] << " <symbol_root_folder>  <library_file>" << std::endl;
    std::cout << "If dump_syms is not put under ${PATH}, export DUMP_SYMS=~/somewhere/dump_syms\n";
    std::cout << argv[0] << " version 201311061349 build at: " << __DATE__ << " " << __TIME__<<std::endl;
    return __LINE__;
  }
  std::string dumpsymcmdpath;
  dumpsymcmdpath = getenv("DUMP_SYMS")?getenv("DUMP_SYMS"):"";
  if (dumpsymcmdpath.length()<1)
  {
    dumpsymcmdpath = run_cmd("which dump_syms");
  }
  if (dumpsymcmdpath.length()<1)
  {
    std::cout << "dump_syms is not found under " << getenv("PATH") << std::endl;    
    std::cout << "dump_syms is not found in environment variable DUMP_SYMS, please export its full path\n";
    return __LINE__;
  }
	if(boost::ends_with(dumpsymcmdpath, "\n"))
		dumpsymcmdpath = dumpsymcmdpath.substr(0, dumpsymcmdpath.size()-1);

//  std::cout << dumpsymcmdpath <<std::endl;
  // dumpsymcmdpath  argv[2] > google_breakpad.sym 
  std::string symfile = "google_breakpad.sym";
//   {
//     std::stringstream ss;
//     ss << dumpsymcmdpath << " " << argv[2] << " > " << symfile;
//     int cmdret = system(ss.str().c_str());
//     if(cmdret)
//     {
//       std::cerr << argv[0] << " error " << ss.str()<<std::endl;
//       return __LINE__;
//     }
//     std::cout << argv[0] << " completed " << ss.str()<<std::endl;
//   } 
// z400-18.verigy.net$ ./storesym aaa ./storesym
// /usr/bin/dump_syms
// /usr/bin/dump_syms
// Usage: /usr/bin/dump_syms [OPTION] <binary-with-debugging-info> [directory-for-debug-file]
// Options:
//   -c    Do not generate CFI section
// ./storesym error /usr/bin/dump_syms
//  ./storesym > google_breakpad.sym
	{
		boost::filesystem::remove(symfile);
		std::fstream fsym;
		fsym.open(symfile.c_str(), std::ios_base::out);
		if (fsym.good()==false)
		{
		std::cerr << argv[0] << " failed to open " << symfile << std::endl;
		return __LINE__;
		}
		std::stringstream ss;
		ss << dumpsymcmdpath << " " << argv[2];

// pid_t pid;
//   int status;
// ACE_ARGV_T argslist;
//   argslist.add(dumpsymcmdpath.c_str());
//   argslist.add(symfile.c_str());  
//   int newargc=argslist.argc();
//   ACE_Argv_Type_Converter cmdline(newargc, argslist.argv());
//   char** newargs=cmdline.get_ASCII_argv();

		const int MAX_BUFFER = 2048;    
		char buffer[MAX_BUFFER];
		FILE *stream =popen(ss.str().c_str(), "r");// popen("grep MemFree /proc/meminfo", "r");
		if (stream){
		while (!feof(stream))
		{
			if (fgets(buffer, MAX_BUFFER, stream) != NULL)
				fsym << buffer;
			//usleep(1);
		}
		pclose(stream);
		std::cout << argv[0] << " finished " << ss.str() << std::endl;
		}  
		else{
			std::cerr << "failed to open " << ss.str() << std::endl;
			return __LINE__;
		}
		std::cout << argv[0] << " created " << symfile<<std::endl;
	}
  
  std::ifstream inf;
  inf.open(symfile.c_str(), std::ios_base::in);
  std::string symfileheadline;
  if(inf.good())
  {
    std::getline(inf, symfileheadline);
  }
  else
  {
    std::cerr <<argv[0]<<" Failed to open " << symfile << std::endl;
    return __LINE__;
  }
  // MODULE Linux x86 9FF67103BE22A83965FF13D5542402D60 storesym
  std::deque<std::string> mtokens;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ");
  tokenizer tokens(symfileheadline, sep);
  for (tokenizer::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter)
       {
	 		mtokens.push_back(*tok_iter);
       }
  if(mtokens.size()!=5)
  {
    std::cerr<<argv[0] << "expect 5 fields, unrecognized pattern: " << symfileheadline << std::endl;
    return __LINE__;
  }
  // create directory structure  
  std::string symfolder;
  {
    std::stringstream ss;
   ss << argv[1];
    if(boost::ends_with(argv[1], "/")==false)
      ss<< "/";
    ss << mtokens[4]<<"/"<<mtokens[3];
    symfolder = ss.str();
//     ss.str("");
//     ss << "mkdir -p " << symfolder;
//     int mkdiret = system(ss.str().c_str());
//     if(mkdiret)
//     {
//       std::cerr << argv[0] << " error " << ss.str()<<std::endl;
//       return __LINE__;
//     }
//     std::cout << argv[0] << " completed " << ss.str()<<std::endl;
	if (!boost::filesystem::exists( symfolder )) 
	{
		if(boost::filesystem::create_directories( symfolder ))
		std::cout << argv[0] << " created " << symfolder<<std::endl;
		else 
		{
		std::cout << argv[0] << " failed to create " << symfolder<<std::endl;
		return __LINE__;
		}
	}
  }
  // move file to 
  {
    std::stringstream ss;
//     ss << "mv " << symfile << " " <<symfolder <<"/"<< mtokens[4] << ".sym " ;    
//     int cmdret = system(ss.str().c_str());
//     if(cmdret)
//     {
//       std::cerr << argv[0] << " error " << ss.str()<<std::endl;
//       return __LINE__;
//     }
//     std::cout << argv[0] << " completed " << ss.str()<<std::endl;
    ss.str("");
    ss << symfolder << "/"<<mtokens[4] << ".sym"; 
    std::string topath = ss.str();
    try{
    boost::filesystem::rename(symfile, topath);
	std::cout << argv[0] <<" moved " << symfile << " to " << topath<<std::endl;
    topath.clear();
    }
    catch(const std::exception& e)
    {
      std::cerr << argv[0] <<" failed to rename " << symfile << " to " << topath<<std::endl;
    }
    if(topath.size())
    {
      boost::filesystem::copy_file(symfile, topath, boost::filesystem::copy_option::overwrite_if_exists);
      std::cout << argv[0] <<" copy " << symfile << " to " << topath<<std::endl;
      boost::filesystem::remove(symfile);
      std::cout << argv[0] <<" remove " << symfile <<std::endl;
    }
  }   
    return 0;
}
// ~/projects/storesym$ DUMP_SYMS=~/projects/google-breakpad/src/tools/linux/dump_syms/dump_syms build/storesym test2/ build/storesym
// depends on libboost_filesystem.so and libboost_system.so
