//============================================================================
// Name        : htmltest.cpp
// Author      : Onega
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "gumbo.h"
#include <sstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <sys/select.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <functional>
#include <curl/curl.h>
#include <fstream>
#include <time.h>
#include <iomanip>
#include <openssl/sha.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <sqlite3.h>
#include <signal.h>
#include <stdlib.h>
#include "url_class.h"
#include "http_downloader.h"
#include "page_parser.h"
#include "resource_database.h"
#include "digestclass.h"

int input_signal=0;
void my_handler(int signalinput) {
	input_signal = signalinput;
	std::cout << __FUNCTION__ << " got signal " << signalinput << std::endl;
	exit(0);
}

boost::program_options::variables_map vm;

int main(int argc, char** argv) {
    using namespace std;
    cout << argv[0] << " pid " << getpid() << " built time "<< __DATE__ <<" " << __TIME__<< endl;
//    {
//    image_digest_t digest, d2;
//    memset(digest.data(), 5, digest.size());
//    memset(d2.data(), 1, digest.size());
//    std::string test = digest_class::digest_to_string(digest);
//    digest_class::string_to_digest(test, d2);
//    std::cout << "digest to string and back " << (memcmp(digest.data(), d2.data(), d2.size())==0?"pass":"fail") << std::endl;
//    }
    namespace po = boost::program_options;
    po::options_description desc("Options");     
    desc.add_options() 
      ("help", "Print help messages") 
      ("dbpath", po::value<std::string>(),  "database file path") 
      ("pageurl", po::value<std::string>(),  "page url to parse")
	  ("pagesite", po::value<std::vector<std::string>>(),  "only urls with those keywords will be checked")
      ("imageurl", po::value<std::string>(),"image url to download")
	  ("minfilesize", po::value<int>(), "minimal files size")
	  ("http-timeout-ms", po::value<int>(), "timeout milliseconds when downloading by http")
	  ("recursive", "process links in the page")
	  ("save-page-src", "save html page source to file")
	  ("image-short-min", po::value<int>(), "both width and height shall be greater than the specified pixel size")
	  ("image-long-min", po::value<int>(), "width or height shall be greater than the specified pixel size")
	  ("page-link-depth", po::value<int>(), "page link depths, similar to TTL")
	  ("check-dup", po::value<std::string>(), "check files under the folder see if digest is duplicated")
	  ("imagefolder", po::value<std::string>(), "specify the directory to save image files")
	  ; 
    
    try 
    { 
      po::store(po::parse_command_line(argc, argv, desc),vm);
      po::notify(vm); 
      if ( vm.count("help")  ) 
      { 
        std::cout << argv[0] << " usage:" << std::endl 
                  << desc << std::endl; 
        return 0; 
      } 
    } 
    catch(po::error& e) 
    { 
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
      std::cerr << desc << std::endl; 
      return __LINE__; 
    }     
        ////           
    std::string dbpathstr = "./htmltest.bin";
    if(vm.count("dbpath"))    {
    	dbpathstr = vm["dbpath"].as<std::string>();
    }
    resource_database db;
    boost::filesystem::path dbpath(dbpathstr);
	if (boost::filesystem::exists(dbpath)) {
		db.open(dbpathstr);
		db.load_database();
	} else {
		db.open(dbpathstr);
		db.create_tables();
	}
	if (vm.count("imageurl")) {
		db.add_image_url(vm["imageurl"].as<std::string>());
	}

	if(vm.count("check-dup")) {
		digest_class digestmachine;
		digestmachine.db_ = &db;
		digestmachine.remove_duplicated_file(vm["check-dup"].as<std::string>());
//		return 0;
	}

	if(vm.count("http-timeout-ms")) {
		db.http_timeout_ms = vm["http-timeout-ms"].as<int>();
	}
      /////////////////////////////////////
    if(vm.count("pageurl"))
    {
        std::string pageurl;
        pageurl = vm["pageurl"].as<std::string>() ;
        if (pageurl.length()) {	  
            url_class u(pageurl);
            std::cout << u.protocol_ << '\t' << u.host_<<std::endl;
            std::cout<<u.path_<<std::endl;
            std::cout<<u.query_<<std::endl;
// 	    page_parser pageparser;
// 	    pageparser.set_resource_database(&db);
//             std::string html_src= page_parser::get_page(u.get_url());
//             if (html_src.length()<8)
//             {
//                 std::cout << argv[0] <<" failed to read " << u.get_url()<<std::endl;
//                 return 0;
//             }
//             std::cout << html_src<<std::endl;
//             pageparser.parse(html_src);
	    db.add_page_url(pageurl, 0);
        }
    }
    if(vm.count("imagefolder")) {
    	std::string imagefolder = vm["imagefolder"].as<std::string>();
    	try {
    	if(!boost::filesystem::exists(imagefolder))
    		boost::filesystem::create_directory(imagefolder);
    	} catch (const std::exception& ex) {
    		std::cout << "failed to create " << imagefolder << " " << ex.what();
    		return 1;
    	}
    }
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags =0;
    sigaction(SIGINT, &sigIntHandler, 0);
    while(input_signal == 0)
    {
	  ////////////////////////////////////////
    	std::deque<std::string> pagelist = db.get_page_list();
    	std::cout << __func__ << " there are " << pagelist.size() << " pages\n";
      for(auto pageurl: pagelist)
	  {
    	  if(input_signal)
    	      break;
    	std::cout << __FUNCTION__ <<" " << pageurl<<std::endl;
	    page_parser pageparser;
	    if(vm.count("recursive"))
	    	pageparser.recursive(true);
	    if(vm.count("save-page-src"))
	    	pageparser.save_html_src_ = true;
	    if(vm.count("page-link-depth"))
	    	pageparser.max_depth_ = vm["page-link-depth"].as<int>();
	    if(vm.count("pagesite")) {
	    	pageparser.pagesite = vm["pagesite"].as<std::vector<std::string>>();
	    }
	    pageparser.set_resource_database(&db);
	    pageparser.parse_page(pageurl, 0);
	    // if succeeded, record used time, depth. record minum depth.
	    // if failed, update retry count.
	    // check debug command, display number of pages processed, number of pages left.
	  } 

      std::deque<std::string> imglist = db.get_img_list();
      std::cout << __func__ << " there are " << imglist.size() << " images\n";
      for(std::deque<std::string>::iterator it=imglist.begin();
	  it!=imglist.end();
	  it++)
	  {
    	  if(input_signal)
    		  break;
	    std::cout << __FUNCTION__ << " " <<*it<<std::endl;
	    http_downloader downloader;
	    if(vm.count("minfilesize")){
	    	downloader.min_file_size(vm["minfilesize"].as<int>());
	    }
	    if(vm.count("image-short-min")) {
	    	downloader.image_short_min(vm["image-short-min"].as<int>());
	    }
	    if(vm.count("image-long-min")) {
	    	downloader.image_long_min(vm["image-long-min"].as<int>());
	    }
	    if(vm.count("imagefolder")) {
	    	downloader.set_image_save_location(vm["imagefolder"].as<std::string>());
	    }
	    downloader.db_ = &db;
	    downloader.download_image(it->c_str());
	    // if succeeded, record used, time,
	    // if failed, update database with fail count.
	  }
      if (pagelist.size()+imglist.size()==0)
	    break;
    }
    cout << argv[0] <<" done."<< endl; //
    return 0;
}

