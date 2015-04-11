#include "stdafx.h"
#include <wx/log.h>
//#include <sstream>
#include <fstream>
#include <boost/date_time.hpp>

using boost::asio::ip::tcp;

#include "http_client.h"
#include "NasdaqParser.h"
struct http_client_pimp
//template<>
//struct Loki::ImplOf<http_client>
{

	std::stringstream m_http_content;
	std::string m_url;
	ihttp_event_ptr listener_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;
	boost::system::error_code error_; 
	std::string filename_;
	boost::posix_time::ptime start_time;
	size_t m_total_read_bytes;
	void Init();
};

void http_client_pimp::Init()
{
	//listener_ = NULL;
	m_total_read_bytes = 0;
	using namespace boost::posix_time;
	start_time = microsec_clock::local_time();
}

void http_client::initiate_connect( tcp::resolver::iterator endpoint_iterator )
{
	wxLogVerbose(wxT("%s process %s"), wxString::FromAscii(__FUNCTION__),
		wxString::FromAscii(d->m_url.c_str()));
	tcp::endpoint endpoint = *endpoint_iterator;
	socket_.async_connect(endpoint,
		boost::bind(&http_client::handle_connect, shared_from_this(),
		boost::asio::placeholders::error, ++endpoint_iterator));
}

http_client::~http_client()
{
	if (d->error_)
	{
		wxLogError(wxT("%s %s error %d (%s), used %d seconds"), 
			wxString::FromAscii(__FUNCTION__), 
            wxString::FromAscii(key().c_str()) , 
			d->error_.value(), 
            wxString::FromAscii(d->error_.message().c_str()),
			elapsed_ms());
	}
	else
	{
		wxLogVerbose(wxT("%s %s finished in %d seconds."), 
            wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(key().c_str()), elapsed_ms());
	}
	if (d)
		delete d;
	d = NULL;
}

void http_client::handle_resolve( const boost::system::error_code& err, 
								 tcp::resolver::iterator endpoint_iterator )
{
	if (!err)
	{
		//wxLogVerbose(wxT("%s %s"), wxString::FromAscii(__FUNCTION__),
		//	wxString::FromAscii(key().c_str()));
		initiate_connect(endpoint_iterator);
	}
	else
	{
		notify(err, __FUNCTION__);
	}
}

void http_client::handle_connect( const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator )
{
	wxLogVerbose(wxT("%s %s"), wxString::FromAscii(__FUNCTION__),
		wxString::FromAscii(key().c_str()));
	if (!err)
	{
		// The connection was successful. Send the request.
		boost::asio::async_write(socket_, d->request_,
			boost::bind(&http_client::handle_write_request, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else if (endpoint_iterator != tcp::resolver::iterator())
	{
		// The connection failed. Try the next endpoint in the list.
		initiate_connect(endpoint_iterator);
	}
	else
	{
		notify(err, __FUNCTION__);
	}
}

void http_client::handle_write_request( const boost::system::error_code& err )
{
	if (!err)
	{
		// Read the response status line.
		boost::asio::async_read_until(socket_, d->response_, "\r\n",
			boost::bind(&http_client::handle_read_status_line, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		notify(err, __FUNCTION__);
	}
}

void http_client::handle_read_status_line( const boost::system::error_code& err )
{
	wxLogVerbose(wxT("%s %s"), wxString::FromAscii(__FUNCTION__),
		wxString::FromAscii(key().c_str()));
	if (!err)
	{
		// Check that response is OK.
		std::istream response_stream(&d->response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			//cout << "Invalid response\n";
			return;
		}
		if (status_code != 200)
		{
			//cout << "Response returned with status code ";
			//cout << status_code << "\n";
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(socket_, d->response_, "\r\n\r\n",
			boost::bind(&http_client::handle_read_headers, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		notify(err, __FUNCTION__);
	}
}

void http_client::handle_read_headers( const boost::system::error_code& err )
{
	wxLogVerbose(wxT("%s %s"), wxString::FromAscii(__FUNCTION__),
		wxString::FromAscii(key().c_str()));
	if (!err)
	{
		// Process the response headers.
		std::istream response_stream(&d->response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		{
		}
		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, d->response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&http_client::handle_read_content, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else
	{
		notify(err, __FUNCTION__);
	}
}

//void http_client::handle_read_content( const boost::system::error_code& err )
//{
//	std::string table_end_tag = "</table>";
//	std::string table_start_tag="<table";
//	const std::string INTERESTED_TOKEN = "_inst_own\">";
//	std::string::size_type table_end_tag_pos = std::string::npos;
//	std::string::size_type table_start_tag_pos = std::string::npos;
//	if (!err)
//	{
//		ss_ << &response_;
//		std::string html = ss_.str();
//		std::string::size_type inst_pos = html.find(INTERESTED_TOKEN);
//		if (inst_pos!=std::string::npos)
//		{
//			table_end_tag_pos = html.find(table_end_tag, inst_pos);
//			table_start_tag_pos = html.rfind(table_start_tag, inst_pos);
//			if (table_end_tag_pos != std::string::npos && table_start_tag_pos != std::string::npos)
//			{
//				cout << __FUNCTION__ << " " << key() << ", length=" << ss_.str().size() << std::endl;
//				std::string table_src = html.substr(table_start_tag_pos, table_end_tag_pos + table_end_tag.length()-table_start_tag_pos);
//				socket_.close();
//				savefile_.open(filename_.c_str());
//				if (savefile_.good() )
//				{
//					savefile_ << table_src;
//					savefile_.close();
//					cout << "successfully get " << filename_ << std::endl;
//					notify(error_);
//				}
//				return;
//			}
//		}
//		// Continue reading remaining data until EOF.
//		boost::asio::async_read(socket_, response_,
//			boost::asio::transfer_at_least(1),
//			boost::bind(&http_client::handle_read_content, this,
//			boost::asio::placeholders::error));
//	}
//	else if (err != boost::asio::error::eof)
//	{
//		socket_.close();
//		error_ = err;
//	}
//	else
//	{
//		socket_.close();
//		savefile_.open(filename_.c_str());
//		if (savefile_.good() )
//		{
//			savefile_ << ss_.str();
//			savefile_.close();
//			cout << "failed to get " << filename_ << std::endl;
//			notify(error_);
//		}
//
//	}
//}

void http_client::handle_read_content(const boost::system::error_code& err)
{
	if (!err)
	{
		d->m_total_read_bytes += d->response_.size();
		// Write all of the data that has been read so far.
		wxLogVerbose(wxT("%s read %d/%d from %s"), 
            wxString::FromAscii(__FUNCTION__),
			d->response_.size(), d->m_total_read_bytes, 
            wxString::FromAscii(key().c_str()));
		d->m_http_content << &d->response_;
        if (d->m_http_content.str().find("</html>")!=std::string::npos
            && d->filename_.size())
        {
            NasdaqParser nasdaq_parser;
            nasdaq_parser.Save(d->m_http_content, d->filename_);
            notify(err, __FUNCTION__);
            socket_.close();
            return;
        }
		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, d->response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&http_client::handle_read_content, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else 
	{
		if (d->filename_.size())
		{
			NasdaqParser nasdaq_parser;
			nasdaq_parser.Save(d->m_http_content, d->filename_);
			notify(err, __FUNCTION__);
		}
	}
}

http_client::http_client( boost::asio::io_service& io_service, const std::string& server, 
						 const std::string& path, const std::string& filename ) 
                         : resolver_(io_service),
	socket_(io_service)
{
	d = new http_client_pimp;
	d->Init();
	d->filename_ =(filename);
	set_url(path);
	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&d->request_);
	request_stream << "GET " << path << " HTTP/1.0\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";
	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	//cout << "get " << path << std::endl;
	if (path.length() && server.length() && d->filename_.length())
	{
		tcp::resolver::query query(server, "http");
		resolver_.async_resolve(query,
			boost::bind(&http_client::handle_resolve, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
	}
}

http_client::http_client(boost::asio::io_service& io_service)
	:resolver_(io_service), socket_(io_service)
{
	d = new http_client_pimp;
	d->Init();
}
void http_client::close()
{
	//io_service_.post(boost::bind(&http_client::close, shared_from_this())); 
	boost::system::error_code ec;
	std::string local_address = socket_.local_endpoint().address().to_v4().to_string(ec);
	std::string remote_address = socket_.remote_endpoint().address().to_string(ec);
	wxLogVerbose(wxT("%s %s<==>%s"), wxString::FromAscii(__FUNCTION__), 
		wxString::FromAscii(remote_address.c_str()), wxString::FromAscii(local_address.c_str()));
	socket_.close();
}

std::string http_client::key()
{
	return d->filename_;
}

void http_client::add_event_listener(ihttp_event_ptr listener)
{
	d->listener_ = listener;
}

boost::system::error_code http_client::get_error() const
{
	return d->error_;
}

ihttp_client_ptr http_client::create( boost::asio::io_service& io_service, 
									 const std::string& server, const std::string& path, 
									 const std::string& filename )
{
	return ihttp_client_ptr(new http_client(io_service, server, path, filename));
}

void http_client::handle_google_content( const boost::system::error_code& err )
{
	std::string table_end_tag = "</table>";
	std::string table_start_tag="<table";
	const std::string tail_token = "_inst_own\">";
	const std::string head_token = "market_data_n_chart_div";
	std::string::size_type table_end_tag_pos = std::string::npos;
	std::string::size_type table_start_tag_pos = std::string::npos;
	std::string::size_type not_found = std::string::npos;
	if (!err)
	{
		d->m_http_content << &d->response_;
		std::string html = d->m_http_content.str();
		std::string::size_type tail_token_pos = html.find(tail_token);
		std::string::size_type head_token_pos = html.find(head_token);
		if ((tail_token_pos!=not_found) && (head_token_pos!=not_found))
		{
			table_end_tag_pos = html.find(table_end_tag, tail_token_pos);
			table_start_tag_pos = html.rfind(table_start_tag, head_token_pos);
			if (table_start_tag_pos!=not_found)
				table_start_tag_pos = html.rfind(table_start_tag, table_start_tag_pos-1);
			if (table_end_tag_pos != not_found && table_start_tag_pos != not_found)
			{
				//cout << __FUNCTION__ << " " << key() << ", length=" << ss_.str().size() << std::endl;
				std::string table_src = html.substr(table_start_tag_pos, table_end_tag_pos 
					+ table_end_tag.length()-table_start_tag_pos);
				socket_.close();
				std::ofstream savefile_;
				savefile_.open(d->filename_.c_str());
				if (savefile_.good() )
				{
					savefile_ << table_src;
					savefile_.close();
					//cout << "successfully get " << filename_ << std::endl;
					notify(d->error_);
				}
				return;
			}
		}
		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, d->response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&http_client::handle_google_content, shared_from_this(),
			boost::asio::placeholders::error));
	}
	else if (err != boost::asio::error::eof)
	{
		socket_.close();
		d->error_ = err;
	}
	else
	{
		socket_.close();
		std::ofstream savefile_;
		savefile_.open(d->filename_.c_str());
		if (savefile_.good() )
		{
			savefile_ << d->m_http_content.str();
			savefile_.close();
			//cout << "failed to get " << filename_ << std::endl;
		}
		notify(d->error_, __FUNCTION__);

	}
}

void http_client::notify( const boost::system::error_code& err, const char* func_name )
{
	d->error_ = err;	
	if (d->error_ && func_name)
	{
		wxLogError(wxT("%s error %d (%s), url: %s, file: %s"), 
			wxString::FromAscii(func_name), err.value(), 
            wxString::FromAscii(err.message().c_str()),
			wxString::FromAscii(d->m_url.c_str()), 
            wxString::FromAscii(d->filename_.c_str()));
	}
	if (d->listener_)
	{
		d->listener_->on_complete( shared_from_this(), err);
	}
}

ihttp_client_ptr http_client::create_http1( boost::asio::io_service& io_service, 
										   const std::string& server, 
										   const std::string& path, 
										   const std::string& filename )
{
    //ptr p( new A); p->self = p; return p;
    http_client* new_http_client = new http_client(io_service);
    ptr pret(new_http_client);
    pret->self = pret;
	//ihttp_client_ptr new_http_client(new http_client(io_service));
	new_http_client->set_filename(filename);
	new_http_client->set_url(path);
	std::ostream request_stream(&(new_http_client->d->request_));
	request_stream << "GET " << path << " HTTP/1.1\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.6)\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Accept-Language: en-us,en;q=0.5\r\n";
	request_stream << "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n";
	request_stream << "Connection: close\r\n\r\n";
	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	//cout << "get " << path << std::endl;
	if (path.length() && server.length() 
        && new_http_client->get_filename().length())
	{
		try
		{
            ihttp_client_ptr ret = pret;
			tcp::resolver::query query(server, "http");
			new_http_client->resolver_.async_resolve(query,
				boost::bind(&http_client::handle_resolve, 
                pret,
                //new_http_client,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
			return ret;
		}
		catch (std::exception& e)
		{
			wxLogError(wxT("%s Exception %s"), wxString::FromAscii(__FUNCTION__), 
				wxString::FromAscii(e.what()));
		}
	}
	else
	{
		wxLogError(wxT("Missing parameter"));
	}
	return ihttp_client_ptr();
}

void http_client::set_url( const std::string& url )
{
	d->m_url = url;
}

int http_client::elapsed_ms()
{
	using namespace boost::posix_time;
	ptime local_time = microsec_clock::local_time();
	boost::posix_time::time_duration used_time = local_time - d->start_time;
	return used_time.total_milliseconds();
}

std::string http_client::get_url()
{
	return d->m_url;
}

std::string http_client::get_filename()
{
	return d->filename_;
}
void http_client::set_filename(const std::string &filename)
{
    d->filename_ = filename;
}
size_t http_client::get_total_read_bytes()
{
return d->m_total_read_bytes;
}

boost::shared_ptr<http_client> http_client::shared_from_this()
{
    //return shared_from_this();  
    return boost::static_pointer_cast<http_client>(ihttp_client::shared_from_this());
}