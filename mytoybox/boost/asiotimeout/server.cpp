#include <iostream>
#include <string>
#include <signal.h>
#include <pthread.h>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/log/trivial.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/phoenix/bind.hpp>
#include <memory>
#include <map>
#include <atomic>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
using boost::asio::deadline_timer;
class async_tcp_connection;
typedef boost::shared_ptr<async_tcp_connection> ptr_async_tcp_connection;

typedef boost::log::sources::severity_channel_logger_mt<
    	logging::trivial::severity_level,     // the type of the severity level
    std::string         // the type of the channel name
> my_logger_mt;

BOOST_LOG_ATTRIBUTE_KEYWORD(module_id, "ModuleID",  int)
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(mylog1, my_logger_mt)
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(mylog2, my_logger_mt)
logging::trivial::severity_level module_severity_table[32];

bool log_level_filter(
		logging::value_ref< logging::trivial::severity_level > const& log_level,
		logging::value_ref<int> const& mid) {
	int aaa = mid.get();
	logging::trivial::severity_level val = log_level.get();
        return val >= module_severity_table[aaa];
    }

void init_logging()
{
    boost::shared_ptr< logging::core > core = logging::core::get();
    boost::shared_ptr< sinks::text_ostream_backend > backend =
        boost::make_shared< sinks::text_ostream_backend >();
    backend->add_stream(
        boost::shared_ptr< std::ostream >(&std::clog, boost::null_deleter()));
    backend->add_stream(
        boost::shared_ptr< std::ostream >(new std::ofstream("sample.log", std::ofstream::app)));
    backend->auto_flush(true);
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));
    core->add_sink(sink);
//    std::for_each( module_severity_table, module_severity_table+32, [](logging::trivial::severity_level& i) { i= logging::trivial::info;} );
    std::fill(module_severity_table, module_severity_table+32, logging::trivial::info);
    module_severity_table[1] = logging::trivial::info;
    module_severity_table[2] = logging::trivial::trace;
    std::cout<<"default log level is info, module 1 is warning, module 2 is trace\n";
    sink->set_formatter
        (
        		boost::log::expressions::stream
        		<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y%m%d %H:%M:%S")
        		<< " "<< boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")
        		                << ": <" << boost::log::trivial::severity
        		                << "> " << boost::log::expressions::smessage
        );
    boost::log::expressions::attr< int >("ModuleID");
	namespace phoenix = boost::phoenix;
	core->set_filter(phoenix::bind(&log_level_filter,
			boost::log::expressions::attr< logging::trivial::severity_level >("Severity"),
			boost::log::expressions::attr< int >("ModuleID").or_default(0)
			));
	mylog1::get().add_attribute("ModuleID", boost::log::attributes::constant< int >(1));
	mylog2::get().add_attribute("ModuleID", boost::log::attributes::constant< int >(2));
	boost::log::add_common_attributes();
}

unsigned short tcp_port = 1234;
std::atomic_uint connection_cnt;
class async_tcp_connection: public boost::enable_shared_from_this<async_tcp_connection>
{
public:
    ~async_tcp_connection() 
    {
    	BOOST_LOG_TRIVIAL(info) << __func__ << " connection count: " << connection_cnt.fetch_sub(1);
    }
    static boost::shared_ptr<async_tcp_connection> create(boost::asio::io_service& io_service)
	{
    	return boost::shared_ptr<async_tcp_connection>(new async_tcp_connection(io_service));
	}
    void start()
    {
        std::cout << __FUNCTION__  << std::endl;
        input_deadline_.async_wait(boost::bind(&async_tcp_connection::check_deadline, shared_from_this(), &input_deadline_));
//        output_deadline_.async_wait(boost::bind(&async_tcp_connection::check_deadline, shared_from_this(), &output_deadline_));
        input_deadline_.expires_from_now(boost::posix_time::seconds(4));
//        async_read_until(socket_,
        async_read(socket_,
            request_buf,
            boost::bind(&async_tcp_connection::handle_read_request,
            shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    boost::asio::ip::tcp::socket& socket() { return socket_; }
private:
    async_tcp_connection(boost::asio::io_service& io_service)
        : socket_(io_service), file_size(0), input_deadline_(io_service), output_deadline_(io_service)
    {
    	BOOST_LOG_TRIVIAL(info) << __func__ << " connection count: " << connection_cnt.fetch_add(1);
    	input_deadline_.expires_at(boost::posix_time::pos_infin);
    	output_deadline_.expires_at(boost::posix_time::pos_infin);
    }
    boost::asio::streambuf request_buf;
    size_t file_size;
    std::ofstream output_file;
    boost::asio::ip::tcp::socket socket_;
    boost::array<char, 40960> buf;
    deadline_timer input_deadline_;
    deadline_timer output_deadline_;
    void handle_read_request(const boost::system::error_code& err, std::size_t bytes_transferred)
    {
        if (err)
        {
            return handle_error(__FUNCTION__, err);
        }
        std::cout << __FUNCTION__ << "(" << bytes_transferred << ")"
            << ", in_avail=" << request_buf.in_avail()
            << ", size=" << request_buf.size()
            << ", max_size=" << request_buf.max_size() <<".\n";
        std::istream request_stream(&request_buf);
        std::string file_path;           
        request_stream >> file_path;
        request_stream >> file_size;
        request_stream.read(buf.c_array(), 2); // eat the "\n\n"
        std::cout << file_path << " size is " << file_size << ", tellg=" << request_stream.tellg()<< std::endl;
        size_t pos = file_path.find_last_of('\\');
        if (pos!=std::string::npos)
            file_path = file_path.substr(pos+1);
        output_file.open(file_path.c_str(), std::ios_base::binary);
        if (!output_file)
        {
            std::cout << "failed to open " << file_path << std::endl;
            return;
        }
        // write extra bytes to file
        do
        {
            request_stream.read(buf.c_array(), (std::streamsize)buf.size());
            std::cout << __FUNCTION__ << " write " << request_stream.gcount() << " bytes.\n";
            output_file.write(buf.c_array(), request_stream.gcount());
        } while (request_stream.gcount()>0);
        do_read();
//        async_read(socket_, boost::asio::buffer(buf.c_array(), buf.size()),
//            boost::bind(&async_tcp_connection::handle_read_file_content,
//            shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));          
    }
    void do_read()
    {
    	input_deadline_.expires_from_now(boost::posix_time::seconds(4));
        async_read(socket_, boost::asio::buffer(buf.c_array(), buf.size()),
            boost::bind(&async_tcp_connection::handle_read_file_content,
            shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    void handle_read_file_content(const boost::system::error_code& err, std::size_t bytes_transferred)
    {
        if (bytes_transferred>0)
        {
            output_file.write(buf.c_array(), (std::streamsize)bytes_transferred);
            std::cout << __FUNCTION__ << " recv " << output_file.tellp() << " bytes."<< std::endl;
            if (output_file.tellp()>=(std::streamsize)file_size)
            {
                return;
            }
        }
        if (err)
        {
            return handle_error(__FUNCTION__, err);
        }
        do_read();
//        async_read(socket_, boost::asio::buffer(buf.c_array(), buf.size()),
//            boost::bind(&async_tcp_connection::handle_read_file_content,
//            shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    void handle_error(const std::string& function_name, const boost::system::error_code& err)
    {
        std::cout << __FUNCTION__ << " in " << function_name <<" due to " << err <<" " << err.message()<< std::endl;
		stop();
		std::cout << __FUNCTION__ << " after stop\n";
    }
    void stop()
    {
    	std::cout << __FUNCTION__ << " start\n";
    	if(!socket_.is_open())
    		return;
		boost::system::error_code ignore_ec;
		socket_.cancel(ignore_ec);
		socket_.close(ignore_ec);
		input_deadline_.cancel();
		output_deadline_.cancel();
    }
    void check_deadline(deadline_timer* deadline)
    {
    	if(!socket_.is_open())
    		return;
    	if(deadline->expires_at()<=deadline_timer::traits_type::now()) 
    	{
			std::cout << __FUNCTION__ << " expired\n";
			stop();
			std::cout << __FUNCTION__ << " done\n";
    	} else {
    		std::cout << __FUNCTION__ << " continue wait\n";
    		deadline->async_wait(boost::bind(&async_tcp_connection::check_deadline, shared_from_this(), deadline));
    	}
    }
};

class async_tcp_server : private boost::noncopyable
{
public:
    async_tcp_server(unsigned short port)
        : acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), true)
    {
    	BOOST_LOG_SEV(mylog1::get(), boost::log::trivial::info) << __func__;
    }
    void start_accept()
    {
		ptr_async_tcp_connection new_connection_ = async_tcp_connection::create(io_service_);
			acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&async_tcp_server::handle_accept, this, new_connection_, boost::asio::placeholders::error));

    }
    void handle_accept(ptr_async_tcp_connection current_connection, const boost::system::error_code& e)
    {
        std::cout << __FUNCTION__ << " " << e << ", " << e.message()<<std::endl;
        if (!e)
        {
            current_connection->start();
            start_accept();
        }
    }
    ~async_tcp_server()
    {
        io_service_.stop();
        BOOST_LOG_SEV(mylog1::get(), boost::log::trivial::info) << __func__;
    }
    void stop()
    {
    	io_service_.stop();
    }
    void run()
    {
    	std::cout << __FUNCTION__ << " start\n";
    	boost::asio::io_service::work work(io_service_);
        boost::asio::signal_set signals(io_service_, SIGINT, SIGTERM);
        signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service_));
    	io_service_.run();
    	std::cout << __FUNCTION__ << " end\n";
    }
    void start_thread()
    {
    	ios = boost::thread(boost::bind(&async_tcp_server::run, this));
    }
    boost::thread ios;
private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
	init_logging();
	// g++ -o server -DBOOST_ALL_DYN_LINK server.cpp /usr/lib/libboost_system.a -lboost_log.dll -lboost_thread.dll
    try
    {
        if (argc==2)
        {
            tcp_port=atoi(argv[1]);
        }
        BOOST_LOG_SEV(mylog1::get(), boost::log::trivial::info) <<argv[0] << " listen on port " << tcp_port << std::endl;
        async_tcp_server *recv_file_tcp_server = new async_tcp_server(tcp_port);
        recv_file_tcp_server->start_thread();
        recv_file_tcp_server->start_accept();
        recv_file_tcp_server->ios.join();
        delete recv_file_tcp_server;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
