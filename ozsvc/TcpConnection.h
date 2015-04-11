#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <loki/Pimpl.h>

class TcpHandler;

class TcpConnection: public boost::enable_shared_from_this<TcpConnection>,
	//private boost::noncopyable,
	private Loki::PimplOf<TcpConnection>::Owner
{
public:
    TcpConnection();
    ~TcpConnection();
	/// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket();


	/// Start the first asynchronous operation for the connection.
	void start();
    void stop();
    void do_read(boost::asio::mutable_buffer& buf);
    void do_write(boost::asio::const_buffer& buf);
    void set_handler(boost::shared_ptr<TcpHandler> tcp_handler);
//private:
	/// Handle completion of a read operation.
	void handle_read(const boost::system::error_code& e,
		std::size_t bytes_transferred);

	/// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e,
        std::size_t bytes_transferred);

    void reset_handler();
};

typedef boost::shared_ptr<TcpConnection> PtrTcpConnection;