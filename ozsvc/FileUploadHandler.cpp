#include "StdAfx.h"
#include "FileUploadHandler.h"
#include "TcpConnection.h"

template<>
struct Loki::ImplOf<FileUploadHandler>
{
	enum {COMMAND_ID = 10003};

	/// Buffer for incoming data.
	boost::array<char, 1024> buffer_;// buffer to store command ID
	__int64 total_bytes_read;
	__int64 file_size;
	std::size_t filename_length;
	std::size_t buf_length;
	std::size_t send_length;
	std::size_t total_bytes_send;
	std::string filename;
	std::ofstream src_file;

	void do_read(PtrTcpConnection tcp_connection);
	void do_write(PtrTcpConnection tcp_connection);
	ImplOf();
};
typedef Loki::ImplOf<FileUploadHandler> PrivateFileUploadHandler;

const int FILE_NAME_LENGTH_FIELD = 4;
const int FILE_CONTENT_LENGTH_FIELD = 8;
const int FIXED_HEAD_LENGTH = FILE_NAME_LENGTH_FIELD + FILE_CONTENT_LENGTH_FIELD;
const std::string FILEUPLOAD_RESPONSE = "FileUploadDone\0";

PrivateFileUploadHandler::ImplOf()
{
	total_bytes_read = 0;
	total_bytes_send = 0;
	file_size = 0;
	filename_length = 0;
	buf_length = 0;
	send_length = 0;	
}

void PrivateFileUploadHandler::do_write(PtrTcpConnection tcp_connection)
{
	if (total_bytes_send < buf_length)
		tcp_connection->do_write(boost::asio::const_buffer(buffer_.data()+total_bytes_send, 
		buf_length-total_bytes_send));
	else
		tcp_connection->reset_handler();

}

void PrivateFileUploadHandler::do_read(PtrTcpConnection tcp_connection)
{
	// first read file name length
	if (total_bytes_read < FIXED_HEAD_LENGTH)
	{
		tcp_connection->do_read(boost::asio::buffer(buffer_.data()+ static_cast<int>(total_bytes_read), 
			FIXED_HEAD_LENGTH-total_bytes_read));
		return;
	}
	if (total_bytes_read==FIXED_HEAD_LENGTH && filename_length<=0)
	{
		memcpy(&filename_length, buffer_.data(), sizeof(filename_length));
		memcpy(&file_size, buffer_.data()+FILE_NAME_LENGTH_FIELD, sizeof(file_size));
	}
	// then read file name
	if (filename_length + FIXED_HEAD_LENGTH> total_bytes_read)
	{
		tcp_connection->do_read(boost::asio::buffer(buffer_.data()+total_bytes_read, 
			filename_length+FIXED_HEAD_LENGTH-total_bytes_read));
		return;
	}
	
	if ((filename_length + FIXED_HEAD_LENGTH == total_bytes_read)&&filename.size()==0)
	{
		filename = std::string(buffer_.data()+FIXED_HEAD_LENGTH, filename_length);
		// file name is available
		src_file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary|std::ios_base::app, _SH_DENYRW);
		if (src_file.bad())
			std::cout << "Failed to open " << filename << std::endl;
		if (src_file.good())
			std::cout << "Open file " << filename << std::endl;
		std::cout << "Errno is " << *_errno() << ", description:" << strerror(errno) << std::endl; EACCES;
		buf_length = 0;
	}
	// read file content
	if (buf_length > 0)
	{
		src_file.write(buffer_.data(), buf_length);
		std::cout << "Current file length = " << src_file.tellp() << std::endl;
	}
	buf_length = 0;
	__int64 total_request_size = FIXED_HEAD_LENGTH + filename_length + file_size;
	if (total_request_size >total_bytes_read )
	{
		int next_read_size = min(total_request_size-total_bytes_read, buffer_.size());
		tcp_connection->do_read(boost::asio::buffer(buffer_.data(), next_read_size));
	}
	else
	{
		src_file.close();
		// file update completed, write response
		strcpy(buffer_.data(), FILEUPLOAD_RESPONSE.c_str());
		buf_length = FILEUPLOAD_RESPONSE.size()+1;
		do_write(tcp_connection);
	}

}

FileUploadHandler::FileUploadHandler(void)
{
}

FileUploadHandler::~FileUploadHandler(void)
{
}

int FileUploadHandler::command_id()
{
	return PrivateFileUploadHandler::COMMAND_ID;
}

void FileUploadHandler::handle_read( boost::shared_ptr<TcpConnection> tcp_connection, 
									const boost::system::error_code& e, std::size_t bytes_transferred )
{
	d->buf_length = bytes_transferred;
	d->total_bytes_read += bytes_transferred;
	d->do_read(tcp_connection);
}

void FileUploadHandler::handle_write( boost::shared_ptr<TcpConnection> tcp_connection, 
									 const boost::system::error_code& e, std::size_t bytes_transferred )
{
	d->buf_length = bytes_transferred;
	d->total_bytes_send += bytes_transferred;
	d->do_write(tcp_connection);
}

void FileUploadHandler::handle_start( boost::shared_ptr<TcpConnection> tcp_connection )
{
	d->do_read(tcp_connection);
}