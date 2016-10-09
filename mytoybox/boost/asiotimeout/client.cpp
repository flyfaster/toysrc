#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/null.hpp>
#include <boost/program_options.hpp>

boost::iostreams::stream< boost::iostreams::null_sink > nullOstream( ( boost::iostreams::null_sink() ) );

using boost::asio::ip::tcp;
class async_tcp_client
{
public:
	std::ostream& getstream() {
#ifndef VERBOSE
//		std::cout.setstate(std::ios_base::badbit);
		return nullOstream;
#endif
		return std::cout;
	}
    async_tcp_client(boost::asio::io_service& io_service,
        const std::string& server, const std::string& path)
        : resolver_(io_service),
        socket_(io_service)
    {
    	getstream() << __func__ << std::endl;
    	delay = 0;
        size_t pos = server.find(':');
        if (pos==std::string::npos)
            return;
        std::string port_string = server.substr(pos+1);
        std::string server_ip_or_host = server.substr(0, pos);

        source_file.open(path.c_str(), std::ios_base::binary | std::ios_base::ate);
        if (!source_file)
        {
//            std::cerr << "failed to open " << path << std::endl;
//            return ;
        } else {
			size_t file_size = source_file.tellg();
			source_file.seekg(0);
			// first send file name and file size to server
			std::ostream request_stream(&request_);
			request_stream << path << "\n"
				<< file_size << "\n\n";
			getstream() << "request size:"<<request_.size()<<std::endl;
        }
        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        tcp::resolver::query query(server_ip_or_host, port_string);
        resolver_.async_resolve(query,
            boost::bind(&async_tcp_client::handle_resolve, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::iterator));
    }
    ~async_tcp_client()
    {
    	getstream() << __func__ << std::endl;
    }
    int delay;
private:
    void handle_resolve(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to the first endpoint in the list. Each endpoint
            // will be tried until we successfully establish a connection.
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                boost::bind(&async_tcp_client::handle_connect, this,
                boost::asio::placeholders::error, ++endpoint_iterator));
        }
        else
        {
            std::cerr << "Error: " << err.message() << "\n";
        }
    }

    void handle_connect(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // The connection was successful. Send the request.
        	getstream() << __func__ << " sleep to test read timeout\n";
        	for(int i=0; i<delay ; i++) {
        		getstream() << __func__ << " sleep " << i << " minutes\n";
        		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        	}
        	if (!source_file) {
				std::cerr << __func__ << " early return because no file to send\n";
				return;
        	}
            boost::asio::async_write(socket_, request_,
                boost::bind(&async_tcp_client::handle_write_file, this,
                boost::asio::placeholders::error));
        }
        else if (endpoint_iterator != tcp::resolver::iterator())
        {
            // The connection failed. Try the next endpoint in the list.
            socket_.close();
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect(endpoint,
                boost::bind(&async_tcp_client::handle_connect, this,
                boost::asio::placeholders::error, ++endpoint_iterator));
        }
        else
        {
            std::cerr << "Error: " << err.message() << "\n";
        }
    }

    void handle_write_file(const boost::system::error_code& err)
    {
        if (!err)
        {
                if (source_file.eof()==false)
                {
                    source_file.read(buf.c_array(), (std::streamsize)buf.size());
                    if (source_file.gcount()<=0)
                    {
                        std::cerr << "read file error " << std::endl;
                        return;
                    }
                    getstream() << "send " <<source_file.gcount()<<" bytes, total:" << source_file.tellg() << " bytes.\n";
                    boost::asio::async_write(socket_,
                        boost::asio::buffer(buf.c_array(), source_file.gcount()),
                        boost::bind(&async_tcp_client::handle_write_file, this,
                        boost::asio::placeholders::error));
                    if (err)
                    {
                        std::cerr << "send error:" << err << std::endl;
                        return;
                    }
                }
                else
                    return;
        }
        else
        {
            std::cerr << "Error: " << err.message() << "\n";
        }

    }
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::array<char, 1024> buf;
    boost::asio::streambuf request_;
    std::ifstream source_file;
};
int main(int argc, char* argv[])
{
	// build in Cygwin: g++ -o client client.cpp /usr/lib/libboost_system.a -lboost_date_time.dll -lboost_thread.dll
	// g++ -o server -DBOOST_ALL_DYN_LINK -std=c++0x -pthread server.cpp /usr/lib/libboost_system.a -lboost_log.dll -lboost_thread.dll
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help", "Print help messages")
          ("server", po::value<std::string>(),  "specify server address and port like 127.0.0.1:1234")
		  ("path", po::value<std::string>(),  "specify file to be sent to server")
      ("delay", po::value<int>(),  "number of seconds the client wait after connect, intended to test maximum number of connections to a server")
	  ;
    boost::program_options::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(),vm);
      po::notify(vm);
      if ( vm.count("help")  )
      {
        std::cout << argv[0] << " usage:" << std::endl
                  << desc << std::endl;
        return 0;
      }
    }
    catch(po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return __LINE__;
    }
    std::string server = "127.0.0.1:1234";
    std::string path;
   if (vm.count("server"))
    {
    	server = vm["server"].as<std::string>();
    }
   if(vm.count("path"))
	   path = vm["path"].as<std::string>();
//	if (argc != 3)
//    {
//        std::cerr << "Usage: " << argv[0] << " <server-address> <file path>" << std::endl;
//        std::cerr << "sample: " << argv[0] << " 127.0.0.1:1234 c:\\tmp\\a.txt" << std::endl;
//        return __LINE__;
//    }
    try
    {
//    	freopen("/dev/null", "w", stdout);
    	std::cout.setstate(std::ios_base::badbit);
        boost::asio::io_service io_service;
        async_tcp_client client(io_service, server.c_str(), path.c_str());
        if(vm.count("delay"))
        	client.delay = vm["delay"].as<int>();
        io_service.run();

        std::cout << argv[0]<< " send file " << path << " completed successfully.\n";
//         system("pause");
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
} 
