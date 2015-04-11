#include "StdAfx.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"

using boost::asio::ip::tcp;

#include "http_initiator.h"
#include "stock_symbol_collection.h"

http_initiator::http_initiator(void)
{
}

http_initiator::~http_initiator(void)
{
}

void http_initiator::on_complete(ihttp_client_p client, boost::system::error_code err ) 
{
	//create_connection();
	clients.erase(client->key());
}

void http_initiator::prepare_request( const std::string& stock_symbol )
{
	using namespace boost::posix_time;
	ptime local_now = second_clock::local_time();
	typedef boost::date_time::local_adjustor<ptime, +12, no_dst> us_eastern;
	ptime ny_now =  us_eastern::local_to_utc(local_now);
	std::stringstream ss;
	ss << stock_symbol 
		<< std::setw(4) << std::setfill('0') << ny_now.date().year()
		<< std::setw(2) << std::setfill('0') << (int)ny_now.date().month()
		<< std::setw(2) << std::setfill('0') << ny_now.date().day()
		<< std::setw(2) << std::setfill('0') << ny_now.time_of_day().hours()
		<< std::setw(2) << std::setfill('0') << ny_now.time_of_day().minutes()
		<< ".htm"
		;
	std::string filename = ss.str();
	extern boost::asio::io_service io_service;
	cout << __FUNCTION__ << " (" << stock_symbol <<") save as " << filename <<std::endl;
	if (stock_symbol.size())
	{
		ihttp_client_ptr client=http_client::create(io_service, 
			"finance.google.com",
			std::string("/finance?q=") + stock_symbol,
			filename );
		client->add_event_listener(this);
		clients[client->key()] = client;
	}

}
void http_initiator::create_connection(void)
{
	std::string stock_symbol = SingleSymbolCol::Instance().pop_symbol();
	prepare_request(stock_symbol);

}
