#pragma once
//#include <loki/Pimpl.h>
#include "ihttp_client.h"

//http://www.gamedev.net/community/forums/topic.asp?topic_id=486872
class STOCKCLIENT_API http_client : //public boost::enable_shared_from_this<http_client>,
	public ihttp_client//, private Loki::PimplOf<http_client>::Owner
{
public:
	~http_client();
    boost::shared_ptr<http_client> shared_from_this();
    //boost::shared_ptr<http_client> shared_from_this() const { return boost::static_pointer_cast<http_client>(ihttp_client::shared_from_this()); }

	boost::system::error_code get_error() const;

	void close();

	std::string key();
	void set_url(const std::string& url);

	void add_event_listener(ihttp_event_ptr listener);
	static ihttp_client_ptr create(boost::asio::io_service& io_service,
		const std::string& server, const std::string& path, const std::string& filename);

	static ihttp_client_ptr create_http1(boost::asio::io_service& io_service,
		const std::string& server, const std::string& path, const std::string& filename);

	int elapsed_ms();
	std::string get_url();
	std::string get_filename();
    void set_filename(const std::string& filename);
	virtual size_t get_total_read_bytes();

private:
	http_client();
	http_client(boost::asio::io_service& io_service);
	http_client(boost::asio::io_service& io_service,
		const std::string& server, const std::string& path, const std::string& filename);

	void initiate_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
	void handle_resolve(const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void handle_connect(const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

	void handle_write_request(const boost::system::error_code& err);

	void handle_read_status_line(const boost::system::error_code& err);

	void handle_read_headers(const boost::system::error_code& err);

	void handle_read_content(const boost::system::error_code& err);

	void handle_google_content(const boost::system::error_code& err);
	void notify(const boost::system::error_code& err, const char* func_name=NULL);
	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	struct http_client_pimp* d;
    typedef boost::shared_ptr<http_client> ptr; 
    typedef boost::weak_ptr<http_client>   wptr; 
    wptr    self; 

};

