#include "StdAfx.h"
#include "CommandHandler.h"
#include "TcpConnection.h"
#include "FileDownloadHandler.h"
#include "FileInfoHandler.h"
#include "FileUploadHandler.h"


template<>
struct Loki::ImplOf<CommandHandler>
{
    static const int COMMAND_ID_LENGTH = 4;
	/// Buffer for incoming data.
	boost::array<char, COMMAND_ID_LENGTH+4> buffer_;// buffer to store command ID
    std::size_t total_bytes_read;

    void do_read(PtrTcpConnection tcp_connection);
};

typedef Loki::ImplOf<CommandHandler> PrivateCommandHandler;

void PrivateCommandHandler::do_read(PtrTcpConnection tcp_connection)
{
    tcp_connection->do_read(
        boost::asio::buffer(buffer_.data()+total_bytes_read, 
        COMMAND_ID_LENGTH-total_bytes_read)
        );

}

CommandHandler::CommandHandler(void)
{
    d->total_bytes_read = 0;
}

CommandHandler::~CommandHandler(void)
{
}

void CommandHandler::handle_read( PtrTcpConnection tcp_connection, const boost::system::error_code& e, std::size_t bytes_transferred )
{
    d->total_bytes_read += bytes_transferred;
    if (d->total_bytes_read < PrivateCommandHandler::COMMAND_ID_LENGTH)
    {
        d->do_read(tcp_connection);
    }
    else
        if (d->total_bytes_read == PrivateCommandHandler::COMMAND_ID_LENGTH)
        {
            // use Factory patter to create a specific handler
            int command_id = 0;
            memcpy(&command_id, d->buffer_.data(), sizeof(command_id));
            if (command_id == 10001)
            {
                tcp_connection->set_handler( PtrTcpHandler(new FileDownloadHandler));
            }
            if (command_id == 10002)
            {
                tcp_connection->set_handler(PtrTcpHandler(new FileInfoHandler));
            }
			if (command_id == 10003)
			{
				tcp_connection->set_handler(PtrTcpHandler(new FileUploadHandler));
			}
        }
        else
        {
            // should not read more than wanted.
            std::stringstream ss;
            ss << __FUNCTION__ << "(" << bytes_transferred << "), total bytes read:" << d->total_bytes_read;
            throw std::runtime_error(ss.str());
        }
}

void CommandHandler::handle_write( PtrTcpConnection tcp_connection, const boost::system::error_code& e,
                                  std::size_t bytes_transferred )
{

}

void CommandHandler::handle_start( PtrTcpConnection tcp_connection )
{
    d->do_read(tcp_connection);
}

int CommandHandler::command_id()
{
    return 10000;
}