#include "StdAfx.h"
#include "FileDownloadHandler.h"
#include <fstream>
#include "CommandHandler.h"
#include "TcpConnection.h"
template<>
struct Loki::ImplOf<FileDownloadHandler>
{
    enum {COMMAND_ID = 10001};

    /// Buffer for incoming data.
    boost::array<char, 1024> buffer_;// buffer to store command ID
    std::size_t total_bytes_read;
    std::size_t filename_length;
    std::size_t file_size;
    std::size_t buf_length;
    std::size_t send_length;
    std::size_t total_bytes_send;
    std::ifstream src_file;
    void do_read(PtrTcpConnection tcp_connection);
    void do_write(PtrTcpConnection tcp_connection);
    ImplOf();
};
typedef Loki::ImplOf<FileDownloadHandler> PrivateFileDownloadHandler;

PrivateFileDownloadHandler::ImplOf()
{
    total_bytes_read = 0;
    filename_length = 0;
    file_size = 0;
    buf_length = 0;
    send_length = 0;
    total_bytes_send = 0;
}

void PrivateFileDownloadHandler::do_read(PtrTcpConnection tcp_connection)
{
    // first read file name length
    if (total_bytes_read < 4)
    {
        tcp_connection->do_read(boost::asio::buffer(buffer_.data()+total_bytes_read, 4-total_bytes_read));
        return;
    }
    // then read file name
    if (filename_length<=0)
    {
        memcpy(&filename_length, buffer_.data(), sizeof(filename_length));
    }
    if (filename_length + 4 > total_bytes_read)
        tcp_connection->do_read(boost::asio::buffer(buffer_.data()+total_bytes_read, filename_length+4-total_bytes_read));
    else
    {
        // file name is available
        if (src_file.is_open()==false)
        {
            buffer_[filename_length+4] = 0;
            src_file.open(buffer_.data()+4);
            src_file.seekg(0, std::ios_base::end);
            file_size = src_file.tellg();
            src_file.seekg(0, std::ios_base::beg);
        }
        do_write(tcp_connection);
    }
}

void PrivateFileDownloadHandler::do_write(PtrTcpConnection tcp_connection)
{
    if (total_bytes_send<sizeof(file_size))
    {
        memcpy(buffer_.data(), &file_size, sizeof(file_size));
        // sending 4 bytes file length
        tcp_connection->do_write(boost::asio::const_buffer(buffer_.data()+total_bytes_send, 
            sizeof(file_size)-total_bytes_send));
        return;
    }
    if (send_length < buf_length)
    {
        tcp_connection->do_write(boost::asio::const_buffer(buffer_.data()+send_length, 
            buf_length-send_length));
        return;
    }
    if (!src_file.eof())
    {
        buf_length = min(buffer_.size(), file_size-total_bytes_send+4);
        src_file.read(buffer_.data(), buf_length);
        send_length = 0;
        tcp_connection->do_write(boost::asio::const_buffer(buffer_.data(), buf_length));
    }
    else
    {
        tcp_connection->reset_handler();
    }
}

FileDownloadHandler::FileDownloadHandler(void)
{
}

FileDownloadHandler::~FileDownloadHandler(void)
{
}

int FileDownloadHandler::command_id()
{
    return 10001;
}

void FileDownloadHandler::handle_read( boost::shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code& e, std::size_t bytes_transferred )
{
    d->total_bytes_read+= bytes_transferred;
    d->do_read(tcp_connection);
}

void FileDownloadHandler::handle_write( boost::shared_ptr<TcpConnection> tcp_connection, 
                                       const boost::system::error_code& e, std::size_t bytes_transferred )
{
    if (bytes_transferred>0)
    {
        d->total_bytes_send += bytes_transferred;
        d->send_length += bytes_transferred;
    }
    d->do_write(tcp_connection);
}

void FileDownloadHandler::handle_start( boost::shared_ptr<TcpConnection> tcp_connection )
{
    d->do_read(tcp_connection);
}