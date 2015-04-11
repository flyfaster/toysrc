#pragma once 
//#include "http_client.h"
#include "ihttp_client.h"

class STOCKCLIENT_API download_thread : public ihttp_event
{
public:
	download_thread();
	~download_thread();
	long run();
	void operator()();
	void stop();
	void notify();
	void on_complete(ihttp_client_p client, boost::system::error_code err);
	void dump();
	struct impl_download_thread* d;
};
