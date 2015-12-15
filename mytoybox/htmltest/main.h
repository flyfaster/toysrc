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
private:
	  boost::shared_ptr<TTransport> socket;
	  boost::shared_ptr<TTransport> transport;
	  boost::shared_ptr<TProtocol> protocol;
	  boost::program_options::variables_map vm;
	  boost::program_options::options_description desc;
};



#endif /* MAIN_H_ */
