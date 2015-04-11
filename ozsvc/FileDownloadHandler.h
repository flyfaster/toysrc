#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <loki/Pimpl.h>
#include "TcpHandler.h"

class TcpConnection;
// request format: command_id(4 bytes), length of file name(4 bytes), file name.
// response format: length of file content(4 bytes), file content.
class FileDownloadHandler: public TcpHandler,
    public boost::enable_shared_from_this<FileDownloadHandler>,
    private Loki::PimplOf<FileDownloadHandler>::Owner
{
public:
    FileDownloadHandler(void);
    ~FileDownloadHandler(void);
    int command_id();
    /// Handle completion of a read operation.
    void handle_read(boost::shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code& e,
        std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void handle_write(boost::shared_ptr<TcpConnection> tcp_connection, const boost::system::error_code& e,
        std::size_t bytes_transferred);

    void handle_start(boost::shared_ptr<TcpConnection> tcp_connection);

};
