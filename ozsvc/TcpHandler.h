#pragma once 
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

class TcpConnection;

class TcpHandler
{
public:
	/// Handle completion of a read operation.
    virtual void handle_read(boost::shared_ptr<TcpConnection> tcp_connection, 
        const boost::system::error_code& e,
		std::size_t bytes_transferred)=0;

	/// Handle completion of a write operation.
	virtual void handle_write(boost::shared_ptr<TcpConnection> tcp_connection, 
        const boost::system::error_code& e, 
        std::size_t bytes_transferred)=0;

    virtual void handle_start(boost::shared_ptr<TcpConnection> tcp_connection) = 0;

    virtual int command_id() = 0;
};

typedef boost::shared_ptr<TcpHandler> PtrTcpHandler;