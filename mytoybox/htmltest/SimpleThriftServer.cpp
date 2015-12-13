/*
 * SimpleThriftServer.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: onega
 */

#include "SimpleThriftServer.h"
#include "CheckSumServer.h"
#include "htmltest_constants.h"

namespace htmltest {

SimpleThriftServer::SimpleThriftServer() {
	port = g_htmltest_constants.CheckSumServiceDefaultPort;
	server = nullptr;
}

SimpleThriftServer::~SimpleThriftServer() {
	std::cout << __func__ << " pid " << getpid() << std::endl;
}

void SimpleThriftServer::setport(unsigned short myport) {
	port = myport;
}

int SimpleThriftServer::startserver(int argc, char **argv) {

  shared_ptr<CheckSumServiceHandler> handler(new CheckSumServiceHandler());
  shared_ptr<TProcessor> processor(new CheckSumServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  server = new  TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  server->serve();
  return 0;
}

} /* namespace htmltest */
