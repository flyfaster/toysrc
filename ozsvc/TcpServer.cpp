#include "StdAfx.h"
#include "TcpServer.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "TcpConnection.h"

static const std::size_t thread_pool_size_=4;
template<>
struct Loki::ImplOf<TcpServer>
{
	ImplOf();
	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
    PtrTcpConnection new_connection;
    void handle_accept(const boost::system::error_code& e);
    void setup();

};
typedef Loki::ImplOf<TcpServer> PrivateTcpServer;

PrivateTcpServer::ImplOf() : acceptor_(io_service_)
{
    std::cout << __FUNCTION__ << std::endl;
}

void PrivateTcpServer::setup()
{
    if (new_connection)
        return;
    boost::asio::ip::tcp::resolver resolver(io_service_);
    //boost::asio::ip::tcp::resolver::query query("0.0.0.0", 1245);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 2245 );

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    new_connection.reset(new TcpConnection());
    acceptor_.async_accept(new_connection->socket(),
        boost::bind(&PrivateTcpServer::handle_accept, this,
        boost::asio::placeholders::error));

}

void PrivateTcpServer::handle_accept(const boost::system::error_code& e)
{
    if (!e)
    {
        new_connection->start();
        new_connection.reset(new TcpConnection());
        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&PrivateTcpServer::handle_accept, this,
            boost::asio::placeholders::error));
    }
}



void TcpServer::run()
{
    d->setup();
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for (std::size_t i = 0; i < thread_pool_size_; ++i)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, &(d->io_service_))));
		threads.push_back(thread);
	}

	// Wait for all threads in the pool to exit.
	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();

}

void TcpServer::stop()
{
	d->io_service_.stop();
}

boost::asio::io_service& TcpServer::service()
{
	return d->io_service_;
}

TcpServer::TcpServer( void )
{
    std::cout << __FUNCTION__ << std::endl;

}

TcpServer::~TcpServer( void )
{
    std::cout << __FUNCTION__ << std::endl;

}