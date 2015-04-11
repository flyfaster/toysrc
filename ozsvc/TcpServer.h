#pragma once
#include <loki/pimpl.h>
#include <loki/Singleton.h>
#include <boost/asio.hpp>
class TcpServer : 
    private boost::noncopyable,
    private Loki::PimplOf<TcpServer>::Owner
{
public:
    TcpServer(void);;
    ~TcpServer(void);;
	void run();
	void stop();
	boost::asio::io_service& service();
};

typedef Loki::SingletonHolder<TcpServer, Loki::CreateUsingNew, Loki::PhoenixSingleton> SingleTcpServer;
