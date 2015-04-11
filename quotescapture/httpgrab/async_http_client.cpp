#include "stdafx.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time.hpp>
#include <boost/enable_shared_from_this.hpp>
using boost::asio::ip::tcp;
#include "async_http_client.h"

async_http_client::async_http_client(boost::asio::io_service& io_service,
               const std::string& url, const std::string& path,
               unsigned long timeout_ms)
               : resolver_(io_service), timer_(io_service),
               socket_(io_service),
               local_file_path_(path),
               timeout_ms_(timeout_ms)
{
    // split url into server_ and path_ as www.boost.org /LICENSE_1_0.txt
    //http://blog.think-async.com/2008/05/boostasio-vs-asio.html
    if (boost::algorithm::starts_with(url, "http://")==false)
        return;
    std::string temp = url.substr(strlen("http:://")-1);
    size_t pos = temp.find("/");
    if (pos == std::string::npos)
        return;
    server_ = temp.substr(0, pos);
    path_ = temp.substr(pos);
    start_time_ = boost::posix_time::microsec_clock::universal_time();
    if (timeout_ms_>0)
        end_time_ = start_time_+ boost::posix_time::milliseconds(timeout_ms_);
    else
        end_time_ = start_time_ + boost::posix_time::minutes(60);

}

void async_http_client::handle_resolve(const boost::system::error_code& err,
                            tcp::resolver::iterator endpoint_iterator)
{
   err_ = err;
   if (end_time_ < boost::posix_time::microsec_clock::universal_time())
   {
       err_ = boost::system::errc::make_error_code(boost::system::errc::timed_out);
       std::cout << __FUNCTION__ << " " << err_ << std::endl;
       return;
   }
   if (!err)
    {
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        tcp::endpoint endpoint = *endpoint_iterator;
        socket_.async_connect(endpoint,
            boost::bind(&async_http_client::handle_connect, shared_from_this(),
            boost::asio::placeholders::error, ++endpoint_iterator));
        start_timer(__FUNCTION__);
    }
    else
    {
        handle_error(__FUNCTION__, err);
    }
}

void async_http_client::handle_connect(const boost::system::error_code& err,
                            tcp::resolver::iterator endpoint_iterator)
{
    err_ = err;
    if (!err)
    {
        // The connection was successful. Send the request.
        boost::asio::async_write(socket_, request_,
            boost::bind(&async_http_client::handle_write_request, shared_from_this(),
            boost::asio::placeholders::error));
        start_timer("async_write");
        return;
    }
    if (err.value()==boost::system::errc::timed_out)
        return;
    if (endpoint_iterator != tcp::resolver::iterator())
    {
        // The connection failed. Try the next endpoint in the list.
        socket_.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        socket_.async_connect(endpoint,
            boost::bind(&async_http_client::handle_connect, shared_from_this(),
            boost::asio::placeholders::error, ++endpoint_iterator));
        start_timer(__FUNCTION__);
    }
    else
    {
        handle_error(__FUNCTION__, err);
    }
}

void async_http_client::handle_write_request(const boost::system::error_code& err)
{
    err_ = err;
    if (!err)
    {
        // Read the response status line.
        boost::asio::async_read_until(socket_, response_, "\r\n",
            boost::bind(&async_http_client::handle_read_status_line, shared_from_this(),
            boost::asio::placeholders::error));
        start_timer(__FUNCTION__);
    }
    else
    {
        handle_error(__FUNCTION__, err);
    }
}

void async_http_client::handle_read_status_line(const boost::system::error_code& err)
{
    err_ = err;
    if (!err)
    {
        // Check that response is OK.
        std::istream response_stream(&response_);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            std::cout << "Invalid response\n";
            return;
        }
        if (status_code != 200)
        {
            std::cout << "Response returned with status code ";
            std::cout << status_code << "\n";
            return;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
            boost::bind(&async_http_client::handle_read_headers, shared_from_this(),
            boost::asio::placeholders::error));
        start_timer(__FUNCTION__);
    }
    else
    {
        handle_error(__FUNCTION__, err);
    }
}

void async_http_client::handle_read_headers(const boost::system::error_code& err)
{
    err_ = err;
    if (!err)
    {
        // Process the response headers.
        std::istream response_stream(&response_);
        std::string header;
        while (std::getline(response_stream, header) && header != "\r")
            std::cout << header << "\n";
        std::cout << "\n";

        // Write whatever content we already have to output.
        if (response_.size() > 0)
            std::cout << &response_;

        // Start reading remaining data until EOF.
        boost::asio::async_read(socket_, response_,
            boost::asio::transfer_at_least(1),
            boost::bind(&async_http_client::handle_read_content, shared_from_this(),
            boost::asio::placeholders::error));
        start_timer(__FUNCTION__);
    }
    else
    {
        handle_error(__FUNCTION__, err);
    }
}

void async_http_client::handle_read_content(const boost::system::error_code& err)
{
    err_ = err;
    if (!err)
    {
        // Write all of the data that has been read so far.
        ss_ << &response_;

        // Continue reading remaining data until EOF.
        boost::asio::async_read(socket_, response_,
            boost::asio::transfer_at_least(1),
            boost::bind(&async_http_client::handle_read_content, shared_from_this(),
            boost::asio::placeholders::error));
        start_timer(__FUNCTION__);
    }
    else if (err != boost::asio::error::eof)
    {
        handle_error(__FUNCTION__, err);
    }
    else
    {
        std::string page_content = ss_.str();
        std::cout << __FUNCTION__ << " read " << page_content.length() << " bytes from "
            << server_ << path_ <<"\n";
        std::ofstream fsave;
        fsave.open(local_file_path_.c_str());
        if (fsave.good())
        {
            fsave<<page_content;
        }
        else
        {
            std::cout << __FUNCTION__ << " failed to read from " << server_ << path_ << "\n";
        }
        fsave.close();
    }
}

bool async_http_client::is_valid()
{
    return server_.length() && path_.length() && local_file_path_.length();
}

void async_http_client::close()
{
    std::cout << __FUNCTION__ << "\n";
    if (socket_.is_open())
    {
        socket_.close();
        socket_.io_service().stop();
    }
}

// void async_http_client::enable_timer()
// {
//     //timer_.cancel();
//     using namespace boost::posix_time;
//     ptime pnow = boost::posix_time::microsec_clock::universal_time();
//     if (pnow > end_time_)
//     {
//         std::cout << __FUNCTION__ << " already timeout\n";
//     }
//     //timer_.expires_at(end_time_);
//     timer_.expires_from_now(milliseconds(timeout_ms_));
//     timer_.async_wait(boost::bind(&async_http_client::timeout_break, shared_from_this()));
// }

const boost::system::error_code& async_http_client::get_error() const
{
    return err_;
}

void async_http_client::timeout_break()
{
    std::cout << __FUNCTION__ << " started at " <<boost::posix_time::to_simple_string(start_time_)  
        << ", timeout in " << timeout_ms_ << " milliseconds"
        << ", should stop at " << end_time_ << std::endl;
    std::cout << "current time is " << boost::posix_time::microsec_clock::universal_time() << std::endl;
    boost::posix_time::ptime pnow = boost::posix_time::microsec_clock::universal_time();
    if (pnow < end_time_)
    {
        return;
    }

    if (!err_)
    {
        err_ = boost::system::errc::make_error_code(boost::system::errc::timed_out);
    }
    close();

}

void async_http_client::handle_error( const std::string& function_name, const boost::system::error_code& err )
{
    std::cout << function_name << " Error: " << err.message() << "\n";

}

boost::shared_ptr<async_http_client> async_http_client::create( boost::asio::io_service& io_service, 
                                         const std::string& url, /* format: http://blog.think-async.com/2008/05/boostasio-vs-asio.html */ 
                                         const std::string& path, /* valid file path */ 
                                         unsigned long timeout_ms )
{
    return boost::shared_ptr<async_http_client>(new async_http_client(io_service, 
        url,
        path,
        timeout_ms));
}

async_http_client::~async_http_client()
{
    std::cout<<__FUNCTION__ <<"\n";
}

void async_http_client::do_resolve(  )
{
    std::ostream request_stream(&request_);
    request_stream << "GET " << path_ << " HTTP/1.0\r\n";
    request_stream << "Host: " << server_ << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    tcp::resolver::query query(server_, "http");
    resolver_.async_resolve(query,
        boost::bind(&async_http_client::handle_resolve, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::iterator));
    start_timer(__FUNCTION__);
}

void async_http_client::handle_timeout( const std::string& function_name )
{
    std::cout << function_name << " invoked " << __FUNCTION__ 
        << " started at " <<boost::posix_time::to_simple_string(start_time_)  
        << ", timeout in " << timeout_ms_ << " milliseconds"
        << ", should stop at " << end_time_ << std::endl;
    std::cout << "current time is " << boost::posix_time::microsec_clock::universal_time() << std::endl;
    boost::posix_time::ptime pnow = boost::posix_time::microsec_clock::universal_time();
    if (pnow < end_time_)
    {
        return;
    }
    if (!err_)
    {
        err_ = boost::system::errc::make_error_code(boost::system::errc::timed_out);
    }
    close();
}

void async_http_client::start_timer( const std::string& function_name )
{    
    using namespace boost::posix_time;
    timer_.expires_from_now(milliseconds(timeout_ms_));
    timer_.async_wait(boost::bind(&async_http_client::handle_timeout, shared_from_this(), function_name));
}