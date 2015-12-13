/*
 * CheckSumServer.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: onega
 */

#include "CheckSumServer.h"
#include <iostream>
namespace htmltest {

CheckSumServer::CheckSumServer():server_(nullptr) {
	// TODO Auto-generated constructor stub

}

CheckSumServer::~CheckSumServer() {
	// TODO Auto-generated destructor stub
}

  void CheckSumServer::checksum(CheckSumResult& _return, const std::string& srcfilename) {
    // Your implementation goes here
    printf("checksum\n");
  }

  int32_t CheckSumServer::duplicated(const std::string& input_checksum) {
    // Your implementation goes here
    std::cout << __func__ << " pid " << getpid() << " " << input_checksum<< std::endl;
  }

  void CheckSumServer::addfolder(const std::string& pathname) {
    std::cout << __func__ << " pid " << getpid() << " " << pathname<< std::endl;
  }

  void CheckSumServer::getprogress(CheckSumProgress& _return) {
	  _return.__set_folders(0);
	  _return.__set_files(0);
	  _return.__set_processed_folders(0);
    // Your implementation goes here
    printf("getprogress\n");
  }

  void CheckSumServer::ping() {
	  std::cout << __func__ << " pid " << getpid() << std::endl;
  }

  void CheckSumServer::shutdown(const std::string& msg) {
    std::cout << __func__ << " pid " << getpid() << ", message from client:" << msg << std::endl;
    if(server_)
    	server_->stop();
  }


} /* namespace htmltest */
