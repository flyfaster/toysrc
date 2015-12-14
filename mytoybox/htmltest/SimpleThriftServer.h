/*
 * SimpleThriftServer.h
 *
 *  Created on: Dec 13, 2015
 *      Author: onega
 */

#ifndef SIMPLETHRIFTSERVER_H_
#define SIMPLETHRIFTSERVER_H_
#include <thrift/server/TSimpleServer.h>
namespace htmltest {

class SimpleThriftServer {
public:
	SimpleThriftServer();
	virtual ~SimpleThriftServer();
	void setport(unsigned short myport);
	int startserver(int argc, char **argv);
	unsigned short port;
	::apache::thrift::server::TSimpleServer* server;
};

} /* namespace htmltest */

#endif /* SIMPLETHRIFTSERVER_H_ */
