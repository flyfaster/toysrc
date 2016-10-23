//============================================================================
// Name        : thriftbidirect.cpp
// Author      : Onega
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <atomic>
#include <iostream>
using namespace std;

#include "HandshakeService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/make_shared.hpp>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include "CallbackService.h"
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <mutex>
#include <deque>
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace apache::thrift::concurrency;

using boost::make_shared;
using boost::shared_ptr;
int display_gap = 100;
int pause_gap = 100; // server pause after pushing every pause_gap notifications

class HandshakeServiceHandler : virtual public HandshakeServiceIf {
 public:
  HandshakeServiceHandler(const boost::shared_ptr<TTransport> &trans)
      : m_client(boost::make_shared<TBinaryProtocol>(trans))
  {
	  create_time = std::chrono::high_resolution_clock::now();
	  std::cout << __func__ << " ctor\n";
      boost::once_flag flag = BOOST_ONCE_INIT;
      m_flag = flag;
  }

  virtual ~HandshakeServiceHandler()
  {
        m_thread->interrupt();
        m_thread->join();
	    auto t2 = std::chrono::high_resolution_clock::now();
	    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - create_time);
        std::cout << __func__ << " dtor after finished "<< cnt.load() << " messages, age " <<int_ms.count() << " milliseconds" <<endl;
  }

  void CallbackThread()
  {
	    static std::atomic<int> pushcnt;
	    int idleloop = 0;
      while(true)
      {
    	  size_t pendingcnt = 0;
          try
          {
              std::string msg;
              {
				  std::lock_guard<std::mutex> guard(push_msg_mutex);
				  if(push_messages.size()>0) {
					  msg = push_messages.front();
					  push_messages.pop_front();
					  pendingcnt = push_messages.size();
				  }
              }
              if(msg.length()) {
            	  m_client.Push(msg);
            	  pushcnt+=1;
              }
          }
      	  catch (TException& tx) {
      	    std::cout <<__func__ << " ERROR: " << tx.what() << std::endl;
      	    return;
      	  }

          if(pendingcnt==0) {
        		  idleloop++; //  start idle counter after first message is received
        	  boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        	  cout << __func__ << " processed " << pushcnt.load() << " messages, idleloop "<< idleloop <<endl;
          } else {
        	  idleloop = 0;
        	  if(pushcnt.load()%pause_gap==(pause_gap-1))
        		  boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
          }
          if(idleloop>10)
          {
        	  break;
          }
      }
      cout << __func__ << " processed " << pushcnt.load() << " messages, idleloop "<< idleloop <<endl;
  }

  void HandShake() {
    cnt+=1;
    if(cnt.load()%display_gap==(display_gap-1))
    	cout<< __func__ << " called " << cnt.load() << " times\n";
    {
		std::lock_guard<std::mutex> guard(push_msg_mutex);
		push_messages.push_back("server push msg " +std::to_string(cnt.load()));
    }
    boost::call_once(boost::bind(&HandshakeServiceHandler::_StartThread, this), m_flag);
  }

  void _StartThread()
  {
    m_thread.reset(new boost::thread(boost::bind(&HandshakeServiceHandler::CallbackThread, this)));
  }

boost::shared_ptr<TTransport> m_trans;
CallbackServiceClient m_client;
std::shared_ptr<boost::thread> m_thread;
boost::once_flag m_flag;
std::mutex push_msg_mutex;
std::deque<std::string> push_messages;
std::atomic<int> cnt;
std::chrono::high_resolution_clock::time_point create_time;
};

class ProcessorFactoryImply : public TProcessorFactory
{
    virtual boost::shared_ptr<TProcessor> getProcessor(
        const TConnectionInfo& connInfo)
    {
        return boost::make_shared<HandshakeServiceProcessor>(boost::make_shared<HandshakeServiceHandler>(connInfo.transport));
    }
};


int main(int argc, char **argv) {
  int port = 5555;
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce help message")
					 ("port", boost::program_options::value<int>( &port)->default_value(port), "Port number to connect")
	  ( "pause_gap", boost::program_options::value<int>(&pause_gap)->default_value(pause_gap),
      "Server pause a little after sending specified number of notifications to client")
	  ( "display_gap", boost::program_options::value<int>(&display_gap)->default_value(display_gap),
      "Server display a message after received specified number of messages from client")
	  ;

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  boost::shared_ptr<TProcessorFactory> processorFactory(new ProcessorFactoryImply());
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  boost::shared_ptr<ThreadManager> threadMgr = ThreadManager::newSimpleThreadManager(30);
  boost::shared_ptr<PlatformThreadFactory> threadFactory =
      boost::shared_ptr<PlatformThreadFactory>(new PlatformThreadFactory());

  threadMgr->threadFactory(threadFactory);
  threadMgr->start();
  TThreadPoolServer server(processorFactory,serverTransport, transportFactory, protocolFactory, threadMgr);
  server.serve();
  return 0;
}

/*
[onega@localhost src]$ /home/onega/src/thrift-0.9.3/bin/thrift --gen cpp -r thriftbidirect.thrift
[onega@localhost src]$ ls
gen-cpp  thriftbidirect.cpp  thriftbidirect.thrift
[onega@localhost src]$ ls gen-cpp/
CallbackService.cpp  CallbackService_server.skeleton.cpp  HandshakeService.h                    thriftbidirect_constants.cpp  thriftbidirect_types.cpp
CallbackService.h    HandshakeService.cpp                 HandshakeService_server.skeleton.cpp  thriftbidirect_constants.h    thriftbidirect_types.h
[onega@localhost src]$
[onega@localhost thriftbidrectclient]$ /home/onega/workspace/cdt/thriftbidirect/Debug/thriftbidirect --pause_gap 40000 --display_gap 10000
HandshakeServiceHandler ctor
HandShake called 9999 times
HandShake called 19999 times
HandShake called 29999 times
CallbackThread processed 10077 messages, idleloop 1
~HandshakeServiceHandler dtor after finished 30000 messages, age 398 milliseconds
*/

