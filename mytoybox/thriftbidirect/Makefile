THRIFT_ROOT=/home/onega/src/thrift-0.9.3

all: thriftbidirect thriftbidirectclient

thriftbidirect: thriftbidirect.cpp thriftbidirect.thrift
	thrift --gen cpp -r thriftbidirect.thrift
	g++ -I${THRIFT_ROOT}/lib/cpp/src -Igen-cpp -o $@ -g -pthread -std=c++11 thriftbidirect.cpp gen-cpp/CallbackService.cpp gen-cpp/HandshakeService.cpp gen-cpp/thriftbidirect_constants.cpp gen-cpp/thriftbidirect_types.cpp -lboost_thread -lboost_program_options -lthrift -lboost_system

thriftbidirectclient: thriftbidirectclient.cpp thriftbidirect.thrift
	thrift --gen cpp -r thriftbidirect.thrift
	g++ -I${THRIFT_ROOT}/lib/cpp/src -Igen-cpp -o $@ -g -pthread -std=c++11 thriftbidirectclient.cpp gen-cpp/CallbackService.cpp gen-cpp/HandshakeService.cpp gen-cpp/thriftbidirect_constants.cpp gen-cpp/thriftbidirect_types.cpp -lboost_thread -lboost_program_options -lthrift -lboost_system

clean:
	@if [ -f thriftbidirect ] ; then rm thriftbidirect ; fi ;
	@if [ -f thriftbidirectclient ] ; then rm thriftbidirectclient ; fi ;




