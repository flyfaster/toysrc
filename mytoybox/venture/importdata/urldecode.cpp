/*
 * urldecode.cpp
 *
 *  Created on: Feb 7, 2015
 *      Author: onega
 */
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>
#include <array>
//#include <dynarray>
#include <boost/shared_array.hpp>
#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
void throw_error(const char * format, ...) {
	std::array<char, 2048> buffer;
	va_list args;
	va_start(args, format);
	vsnprintf(buffer.data(), buffer.size(), format, args);
	va_end(args);
	throw std::runtime_error(buffer.data());
}
namespace url_code {
    std::string encode(const std::string &url);
    std::string decode(const std::string &encoded);

    std::string to_hex(unsigned char c);
    unsigned char to_char(const std::string &str);

    std::string encode(const std::string &src) {
        std::ostringstream out;
        for( auto& c : src)
        {
            short t = c;
            if(
            	isalnum (t) ||
                t == 45 ||          // hyphen
                t == 95 ||          // underscore
                t == 126            // tilde
            ) {
                out << c;
            } else {
                out << to_hex(c);
            }
        }
        return out.str();
    }

std::string decode(const std::string &src) {
	std::ostringstream out;
	std::string::size_type i = 0, len = src.length();
	try {
		for (i = 0; i < len; ++i) {
			if (src.at(i) == '%') {
				std::string str(src.substr(i + 1, 2));
				out << to_char(str);
				i += 2;
			} else {
				out << src.at(i);
			}
		}
	} catch (const std::exception& e) {
		std::cout << "error at " << i << " " << e.what() << std::endl;
	}
	return out.str();
}

    std::string to_hex(unsigned char c) {
        short i = c;
        std::stringstream s;
        s << "%" << std::setw(2) << std::setfill('0') << std::hex << i;
        return s.str();
    }

    unsigned char to_char(const std::string &str) {
        short c = 0;
        if(!str.empty()) {
            std::istringstream in(str);
            in >> std::hex >> c;
            if(in.fail()) {
            	throw_error("decode error. input lenght %d, current pos:%d", str.length(), in.tellg());
            }
        }
        return static_cast<unsigned char>(c);
    }
}

void init(const std::string& log_level, const std::string& logfilename) {
	boost::log::add_file_log(logfilename);
	auto ll = boost::log::trivial::info;
	if (log_level == "trace") {
		ll = boost::log::trivial::trace;
	}
	if (log_level == "info") {
		ll = boost::log::trivial::info;
	}
	if (log_level == "debug") {
		ll = boost::log::trivial::debug;
	}
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= ll);
//	init_term();
}

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")
			("decode_file_name,d", po::value<std::string>(),"set path of input file to decode")
			("size,s", po::value<int>(),"set max size to decode")
			("log_level,l",
			po::value<std::string>(), "set log level");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		BOOST_LOG_TRIVIAL(info)<< desc << "\n";
		return 1;
	}
	std::chrono::system_clock::time_point now =
			std::chrono::system_clock::now();

	{
		std::stringstream ss;
		time_t tt = std::chrono::system_clock::to_time_t(now);
		tm local_tm = *localtime(&tt);
		std::chrono::system_clock::time_point secclock =
				std::chrono::system_clock::from_time_t(tt);
		std::chrono::milliseconds msec = std::chrono::duration_cast
				< std::chrono::milliseconds > (now - secclock);

		ss << argv[0] << "-" << local_tm.tm_year + 1900 << std::setw(2)
				<< std::setfill('0') << local_tm.tm_mon + 1 << std::setw(2)
				<< std::setfill('0') << local_tm.tm_mday << std::setw(2)
				<< std::setfill('0') << local_tm.tm_hour << std::setw(2)
				<< std::setfill('0') << local_tm.tm_min << std::setw(2)
				<< std::setfill('0') << local_tm.tm_sec << std::setw(2)
				<< std::setfill('0') << msec.count()
//       << std::setw(4) << std::setfill('0') << getpid()
				<< ".log";
		std::string logfilename;
		logfilename = ss.str();
		init(
				vm.count("log_level") ?
						vm["log_level"].as<std::string>() : "warning",
				logfilename);
	}

	BOOST_LOG_TRIVIAL(info)<< argv[0] << " pid " << getpid() << " start \n";

	if (vm.count("decode_file_name")==0) {
		std::cout<< desc << "\n";
		return 0;
	}
	std::string inputfile = vm["decode_file_name"].as<std::string>();
	std::ifstream file( inputfile, std::ios::binary | std::ios::ate);
	size_t len = file.tellg();
	boost::shared_array<char> contentbuf(new char[len+4]);
	file.seekg(0, std::ios_base::beg);
	file.read(contentbuf.get(), len);
	contentbuf[len]=0;
	std::string encodedstr(contentbuf.get(), len);
	size_t pos = encodedstr.find("flashvars=", 0); // width="100%" will cause exception
	size_t rightpos = encodedstr.find("corpActionsArray=");
	encodedstr = encodedstr.substr(pos, rightpos-pos);

	int maxsize = encodedstr.length();
	if (vm.count("size"))
		maxsize = vm["size"].as<int>();
	if (maxsize<encodedstr.length())
		encodedstr = encodedstr.substr(0, maxsize);
	BOOST_LOG_TRIVIAL(debug)  << "encode string length " << encodedstr.length()<< std::endl << std::endl;
	std::string decodedstr = url_code::decode(encodedstr);
	BOOST_LOG_TRIVIAL(debug) <<"first round decode result:\n" << decodedstr << std::endl<< std::endl;

	decodedstr = url_code::decode(decodedstr);
		std::cout << decodedstr << std::endl<< std::endl;
	return 0;
}
