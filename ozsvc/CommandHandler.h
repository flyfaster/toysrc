#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <loki/Pimpl.h>
#include "TcpHandler.h"

class CommandHandler: public TcpHandler,
	public boost::enable_shared_from_this<CommandHandler>,
	private Loki::PimplOf<CommandHandler>::Owner
{
public:
	CommandHandler(void);
	~CommandHandler(void);

	/// Handle completion of a read operation.
	void handle_read(boost::shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code& e,
		std::size_t bytes_transferred);

	/// Handle completion of a write operation.
    void handle_write(boost::shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code& e,
        std::size_t bytes_transferred);
	//void set_connection(class TcpConnection* tcp_connection);

    void handle_start(boost::shared_ptr<TcpConnection> tcp_connection);

    int command_id();
};

typedef boost::shared_ptr<CommandHandler> PtrCommandHandler;