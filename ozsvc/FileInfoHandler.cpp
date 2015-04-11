#include "StdAfx.h"
#include "FileInfoHandler.h"
#include "TcpConnection.h"
#include <boost/crc.hpp>

struct FileInfo
{
    std::size_t file_size;
    time_t last_modify;
    DWORD crc32;
};

template<>
struct Loki::ImplOf<FileInfoHandler>
{
    enum {COMMAND_ID = 10002};
    /// Buffer for incoming data.
    boost::array<char, 1024> buffer_;// buffer to store command ID
    std::string file_name;
    std::size_t total_bytes_read;
    std::size_t filename_length;
    std::size_t total_bytes_send;
    ImplOf();
    void do_read(PtrTcpConnection tcp_connection);
    void do_write(PtrTcpConnection tcp_connection);
    static int get_file_time(LPCSTR file_name, FileInfo& file_info);
    static int get_crc(LPCSTR file_name, DWORD& crc);
};
typedef Loki::ImplOf<FileInfoHandler> PrivateFileInfoHandler;

PrivateFileInfoHandler::ImplOf()
{
    total_bytes_read = 0;
    filename_length = 0;
    total_bytes_send = 0;
}

int PrivateFileInfoHandler::get_crc(LPCSTR file_name, DWORD& crc)
{
    std::ifstream output_file(file_name, 
        std::ios_base::_Nocreate|std::ios_base::binary);
    boost::crc_32_type crc_32;
    if (output_file.good())
    {
        do 
        {
            char buf[1024];
            output_file.read(buf, sizeof(buf));
            crc_32.process_bytes(buf, output_file.gcount());
        } while(output_file.good());
        crc = crc_32.checksum();
    }
    return 0;
}

int PrivateFileInfoHandler::get_file_time(LPCSTR file_name, FileInfo& file_info)
{
    int fd = -1;
    fd = _open(file_name, _O_RDONLY);
    if (fd != -1)
    {
        struct _stat file_stat;
        int result = _fstat(fd, &file_stat);
        if (result != 0)
        {
            // failed to get file status
            return result;
        }
        else
        {
            file_info.file_size = file_stat.st_size;
            file_info.last_modify = file_stat.st_mtime;
        }
        _close(fd);
        fd = -1;
    }
    else
    {
        //failed to open file
    }
    return 0;

}

void PrivateFileInfoHandler::do_read(PtrTcpConnection tcp_connection)
{
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
        FileInfo file_info;
        memset(&file_info, 0, sizeof(file_info));
        buffer_[filename_length+4] = 0;
        file_name = buffer_.data() + 4;
        int ret = get_file_time(file_name.c_str(), file_info);
        // get CRC32
        ret = get_crc(file_name.c_str(), file_info.crc32);
        memcpy(buffer_.data(), &file_info, sizeof(file_info));
        do_write(tcp_connection);
    }

}

void PrivateFileInfoHandler::do_write(PtrTcpConnection tcp_connection)
{
    if (total_bytes_send < sizeof(FileInfo))
        tcp_connection->do_write(boost::asio::const_buffer(buffer_.data()+total_bytes_send,
            sizeof(FileInfo)-total_bytes_send));
    else
    {
        tcp_connection->reset_handler( );
    }

}

FileInfoHandler::FileInfoHandler(void)
{
}

FileInfoHandler::~FileInfoHandler(void)
{
    std::cout << __FUNCTION__ << " processed " << d->file_name << std::endl;
}

int FileInfoHandler::command_id()
{
    return PrivateFileInfoHandler::COMMAND_ID;
}

void FileInfoHandler::handle_read( boost::shared_ptr<TcpConnection> tcp_connection, 
                                  const boost::system::error_code& e, std::size_t bytes_transferred )
{
    d->total_bytes_read += bytes_transferred;
    d->do_read(tcp_connection);

}

void FileInfoHandler::handle_write( boost::shared_ptr<TcpConnection> tcp_connection, 
                                   const boost::system::error_code& e, std::size_t bytes_transferred )
{
    d->total_bytes_send += bytes_transferred;
    d->do_write(tcp_connection);
}

void FileInfoHandler::handle_start( boost::shared_ptr<TcpConnection> tcp_connection )
{
    d->do_read(tcp_connection);

}