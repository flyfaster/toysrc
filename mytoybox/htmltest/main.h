/*
 * main.h
 *
 *  Created on: Dec 14, 2015
 *      Author: onega
 */

#ifndef MAIN_H_
#define MAIN_H_
#include "CheckSumService.h"
#include <boost/program_options.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <iostream>
#include <unordered_map>
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class MainApp {
public:
	MainApp();
	~MainApp();
	int Init(int argc, char* argv[]);
	int Start(int argc, char* argv[]);
	int InitThrift();
	static MainApp* Instance();
	boost::program_options::variables_map vm;
	std::unordered_map<std::string, int> exclude_list;
	std::deque<std::string> imagelist;
	int imageposition;
	std::string GetNext();
	std::string GetPrev();
private:
	  boost::shared_ptr<TTransport> socket;
	  boost::shared_ptr<TTransport> transport;
	  boost::shared_ptr<TProtocol> protocol;
	  boost::program_options::options_description desc;

};



#endif /* MAIN_H_ */
