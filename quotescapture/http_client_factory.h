#pragma once
#include "http_client.h"

class STOCKCLIENT_API http_client_factory : 
    //public boost::enable_shared_from_this<http_client_factory>,
	public ihttp_event
{
public:
	http_client_factory();
	~http_client_factory(void);
    boost::shared_ptr<http_client_factory> shared_from_this();

	void on_complete(ihttp_client_p client, boost::system::error_code err);
	void prepare_google_request(const std::string& symbol);
	void prepare_nasdaq_request(const std::string& stock_symbol);
	void prepare_nasdaq_request(const std::string& stock_symbol, int page_no);

    static void build_file_name( char* filename_buf, 
        int filename_buf_length, const std::string &stock_symbol );
    size_t active_connection_count();

	void create_connection(void);
	void add_task(const std::string& symbol, int page_no);
	void add_total_task(const std::string& symbol, int total_page_no);
	bool start_task();
	size_t task_queue_size();
	void add_http_event_listener(ihttp_event_p plistener);
	void dump();
	void check_dead_tasks();
	struct impl_http_client_factory* d;
    boost::weak_ptr<http_client_factory>   self; 

};
