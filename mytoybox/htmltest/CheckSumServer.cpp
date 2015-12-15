/*
 * CheckSumServer.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: onega
 */

#include "CheckSumServer.h"
#include <iostream>
#include "digestclass.h"
#include "boost/filesystem.hpp"

namespace htmltest {

CheckSumServer::CheckSumServer():server_(nullptr) {
	// TODO Auto-generated constructor stub

}

CheckSumServer::~CheckSumServer() {
	// TODO Auto-generated destructor stub
}

  void CheckSumServer::checksum(CheckSumResult& _return, const std::string& srcfilename) {
	  image_digest_t sha1digest;
	  digest_class::sha1checksum(srcfilename, sha1digest);
	  std::string sha1digeststr = digest_class::digest_to_string(sha1digest);
	  _return.__set_status(0);
	  _return.__set_checksum(sha1digeststr);
	  image_digest_table.insert(sha1digest);
  }

  int32_t CheckSumServer::duplicated(const std::string& input_checksum) {
    std::cout << __func__ << " pid " << getpid() << " " << input_checksum<< std::endl;
    image_digest_t output;
    digest_class::string_to_digest(input_checksum, output);
    if(image_digest_table.find(output)!=image_digest_table.end())
    	return 1;
    image_digest_table.insert(output);
    return 0;
  }

  void CheckSumServer::addfolder(const std::string& pathname) {
    std::cout << __func__ << " pid " << getpid() << " " << pathname<< std::endl;
	boost::filesystem::path p (pathname);
	boost::filesystem::recursive_directory_iterator end_itr;
	image_digest_t digest;
    // cycle through the directory
    for (boost::filesystem::recursive_directory_iterator itr(p); itr != end_itr; ++itr)
    {
        // If it's not a directory, list it. If you want to list directories too, just remove this check.
        if (boost::filesystem::is_regular_file(itr->path())) {
            // assign current file name to current_file and echo it out to the console.
            std::string current_file = itr->path().string();
            int ret = digest_class::sha1checksum(current_file, digest);
            if(image_digest_table.find(digest)!=image_digest_table.end()) {
            	std::cout << __METHOD_NAME__ << " found duplicate " << current_file << std::endl;
            } else {
            image_digest_table.insert(digest);
            }
        }
    }
    std::cout << __METHOD_NAME__ << " processed " << pathname << ", checksum count:"<<image_digest_table.size() << std::endl;
  }

  void CheckSumServer::getprogress(CheckSumProgress& _return) {
	  std::cout << __METHOD_NAME__ << " pid " << getpid() << " start." << std::endl;
	  _return.__set_folders(0);
	  _return.__set_files(0);
	  _return.__set_processed_folders(0);
	  std::cout << __METHOD_NAME__ << " pid " << getpid() << " done." << std::endl;
  }

  void CheckSumServer::ping() {
	  std::cout << __METHOD_NAME__ << " pid " << getpid() << " done." << std::endl;
  }

  void CheckSumServer::shutdown(const std::string& msg) {
    std::cout << __METHOD_NAME__ << " pid " << getpid() << ", message from client:" << msg << std::endl;
    if(server_)
    	server_->stop();
  }

void CheckSumServer::setserver(class TSimpleServer* server) {
	server_ = server;
}

} /* namespace htmltest */

void htmltest::CheckSumServer::datachecksum(std::string& _return,
		const std::string& data) {
	std::cout << __func__ << " pid " << getpid() << std::endl;
}
