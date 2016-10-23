//============================================================================
// Name        : thriftbidrectclient.cpp
// Author      : Onega
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "CallbackService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <sys/types.h>
#include <unistd.h>
#include "HandshakeService.h"

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

int max_message_count = 1000;
int display_gap = 100;

class CallbackServiceHandler : virtual public CallbackServiceIf {
 public:
  CallbackServiceHandler() {
    cout << __func__ << " pid " << getpid() << endl;
  }

  void Push(const std::string& msg) {
	  CallbackServiceHandler::count(1);
	  if((CallbackServiceHandler::count(0)%display_gap)==0)
		  cout << __func__ << " id " << CallbackServiceHandler::count(0) << " message is " << msg << endl;
  }
  static int count(int add) {
	  static std::atomic<int> msgcnt;
	  msgcnt += add;
	  return msgcnt.load();
  }
  static bool done() {
	  return count(0) >= (max_message_count);
  }
};

boost::mutex io_mutex;
bool worker_is_done = false;
boost::condition_variable condition;

static void process_callback_proc(boost::shared_ptr<protocol::TProtocol> protocol) {
	// process notification from server
	boost::shared_ptr<CallbackServiceIf> callbackif(
			new CallbackServiceHandler());
	boost::shared_ptr<::apache::thrift::TProcessor> processor(
			new CallbackServiceProcessor(callbackif));
	try {
		while (processor->process(protocol, protocol, nullptr)) {
			if (CallbackServiceHandler::done())
				break;
		};
		cout << __func__ << " recevied " << CallbackServiceHandler::count(0)
				<< " messages\n";
		return;
	} catch (TException& tx) {
		std::cout << __func__ << " ERROR: " << tx.what() << std::endl;
		return;
	}
}

static void MonitorThread(boost::shared_ptr<TTransport> trans)
{
    while ( CallbackServiceHandler::done()==false)
    {
        try
        {
            if (!trans->peek())
            {
                cout << __func__ << " connection disconnected, recevied " << CallbackServiceHandler::count(0) << " messages\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        catch (exception& ex)
        {
            cout << __func__ << " " << ex.what();
            return;
        }
    }
}
int main(int argc, char** argv) {
	  string host = "localhost";
	  int port = 5555;
	  boost::program_options::options_description desc("Allowed options");
	  desc.add_options()("help,h",
	                     "produce help message")
						 ("host",
						 boost::program_options::value<string>(&host)
							 ->default_value(host),
						 "Host to connect")
						 ("port",
								boost::program_options::value<int>(
									&port)->default_value(port),
								"Port number to connect")
		  (
	      "msg_cnt",
	      boost::program_options::value<int>(&max_message_count)->default_value(max_message_count),
	      "Number of messages to communicate between server and client");

	  boost::program_options::variables_map vm;
	  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	  boost::program_options::notify(vm);
	  display_gap = max_message_count/10;
	  cout << "Try thrift bi-direction communication" << endl;
	  boost::shared_ptr<TTransport> socket(new TSocket(host, port));
	  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	  HandshakeServiceClient client(protocol);
	  try {
	    transport->open();
	    boost::thread process_thread(boost::bind(&process_callback_proc, protocol) );
//	    boost::thread monitor_thread(boost::bind(&MonitorThread, transport) );
	    auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < max_message_count; ++i)
        {
    	    client.HandShake();
        }
	    auto t2 = std::chrono::high_resolution_clock::now();
	    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        cout << "finished sending " << max_message_count << " messages in " << int_ms.count() << " milliseconds"  << endl;
//        monitor_thread.join();
//        cout << "monitor_thread  done\n";
//        transport->close();
        process_thread.join();
	    auto t3 = std::chrono::high_resolution_clock::now();
	    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1);
        cout << "process_thread  done in " << int_ms.count() << " milliseconds"  << endl;
	  }
	  catch (TException& tx) {
	    std::cout <<__func__ << " ERROR: " << tx.what() << std::endl;
	  }
	return 0;
}
/*
[onega@localhost thriftbidrectclient]$ /home/onega/workspace/cdt/thriftbidrectclient/Debug/thriftbidrectclient --msg_cnt 30000
Try thrift bi-direction communication
CallbackServiceHandler pid 25035
Push id 3000 message is server push msg 3000
finished sending 30000 messages in 122 milliseconds
Push id 6000 message is server push msg 6000
Push id 9000 message is server push msg 9000
Push id 12000 message is server push msg 12000
Push id 15000 message is server push msg 15000
Push id 18000 message is server push msg 18000
Push id 21000 message is server push msg 21000
Push id 24000 message is server push msg 24000
Push id 27000 message is server push msg 27000
Push id 30000 message is server push msg 30000
process_callback_proc recevied 30000 messages
process_thread  done in 273 milliseconds
*/


