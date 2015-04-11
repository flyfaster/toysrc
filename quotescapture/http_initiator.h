#pragma once
#include "http_client.h"

class http_initiator :
	public ihttp_event
{
public:
	http_initiator();
	~http_initiator(void);
	void on_complete(ihttp_client_p client, boost::system::error_code err);
	void prepare_request(const std::string& symbol);
private:
	//stock_symbol_collection symbols_;
public:
	void create_connection(void);
	std::map<std::string, ihttp_client_ptr> clients;

};
