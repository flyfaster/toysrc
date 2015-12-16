/*
 * SimpleThriftServer.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: onega
 */
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "SimpleThriftServer.h"
#include "CheckSumServer.h"
#include "htmltest_constants.h"
#include <iostream>

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

  shared_ptr<CheckSumServer> handler(new CheckSumServer());
  shared_ptr<TProcessor> processor(new CheckSumServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  server = new  TSimpleServer(processor, serverTransport, transportFactory, protocolFactory);
  handler->setserver(server);
  std::cout << argv[0] << " pid " << getpid() << " thrift server start " << std::endl;
  server->serve();
  std::cout << argv[0] << " pid " << getpid() << " thrift server stop " << std::endl;
  return 0;
}

} /* namespace htmltest */
