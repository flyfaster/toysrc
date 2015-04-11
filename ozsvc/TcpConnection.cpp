#include "StdAfx.h"
#include "TcpConnection.h"
#include <boost/bind.hpp>
#include "TcpServer.h"
#include "TcpHandler.h"
#include "CommandHandler.h"
const std::size_t MAX_MESSAGE_SIZE = 1024 * 1024 *256;
template<>
struct Loki::ImplOf<TcpConnection>
{
	/// Strand to ensure the connection's handlers are not called concurrently.
	boost::asio::io_service::strand strand_;

	/// Socket for the connection.
	boost::asio::ip::tcp::socket socket_;

	/// Buffer for incoming data.
	boost::array<char, 4> buffer_;// buffer to store command ID
    int total_read_bytes_;
    PtrTcpHandler active_handler_;
	ImplOf();
};
typedef Loki::ImplOf<TcpConnection> PrivateTcpConnection;

PrivateTcpConnection::ImplOf() : strand_(SingleTcpServer::Instance().service()),
socket_(SingleTcpServer::Instance().service())
{
total_read_bytes_=0;
active_handler_ = PtrTcpHandler(new CommandHandler);
}


void TcpConnection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
    std::cout << __FUNCTION__ << std::dec << "(" <<e << ", " << bytes_transferred << ")" << std::endl;
	if (!e && d->active_handler_)
		d->active_handler_->handle_read(shared_from_this(), e, bytes_transferred);
}

void TcpConnection::handle_write(const boost::system::error_code& e,
                                 std::size_t bytes_transferred)
{
    std::cout << __FUNCTION__ << std::dec << "(" <<e << ", " << bytes_transferred << ")" << std::endl;
    if (!e && d->active_handler_)
        d->active_handler_->handle_write(shared_from_this(), e, bytes_transferred);

}


void TcpConnection::start()
{
	if (d->active_handler_)
        d->active_handler_->handle_start(shared_from_this());
}

boost::asio::ip::tcp::socket& TcpConnection::socket()
{
	return d->socket_;
}


void TcpConnection::do_read( boost::asio::mutable_buffer& buf )
{
	d->socket_.async_read_some(
		boost::asio::mutable_buffers_1(buf),
		d->strand_.wrap(
		boost::bind(&TcpConnection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));

}

void TcpConnection::do_write( boost::asio::const_buffer& buf )
{
	d->socket_.async_write_some(
        boost::asio::const_buffers_1(buf),
		d->strand_.wrap(
		boost::bind(&TcpConnection::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred)));

}

void TcpConnection::set_handler( PtrTcpHandler tcp_handler )
{
    std::stringstream ss;
    ss << __FUNCTION__ << "(" << typeid(*tcp_handler).name() << ")";
    OutputDebugStringA(ss.str().c_str());
    d->active_handler_ = tcp_handler;
    if (tcp_handler)
        tcp_handler->handle_start(shared_from_this());
}

void TcpConnection::stop()
{
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    d->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    d->socket_.close();
}

TcpConnection::TcpConnection()
{
    std::cout << __FUNCTION__ << " " << std::hex << std::setfill('0') << std::setw(8) 
        << std::internal << (int)this << std::endl;
}

TcpConnection::~TcpConnection()
{
    std::cout << __FUNCTION__ << " " << std::hex << std::setfill('0') << std::setw(8) 
        << std::internal << (int)this << std::endl;
}

void TcpConnection::reset_handler()
{
    set_handler( PtrTcpHandler(new CommandHandler));
}