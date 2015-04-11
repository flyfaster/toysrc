//vc access postgresql8.0 bool compatible

#include <sql.h>	// yum install unixODBC-devel
#include <sqlext.h>
#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <deque>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <exception>
#include <stdarg.h>
#include <array>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_array.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/timer/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <client/linux/dump_writer_common/mapping_info.h>
#include <client/linux/handler/exception_handler.h>
#include "odbctool.h"
#include "daytrend.h"
//Onega 2005/02/15
//build on windows 2003 with VC2003, PostgreSQL 8.0
//bool column may be SQL_CHAR and '1'(ASCII 49) = true, '0'= false
//Onega 2015
//build on CentOS with gcc 4.9.2

void throw_error(const char * format, ...) {
	std::array<char, 2048> buffer;
	va_list args;
	va_start(args, format);
	vsnprintf(buffer.data(), buffer.size(), format, args);
	va_end(args);
	throw std::runtime_error(buffer.data());
}

namespace ODBCAgent {
SQLRETURN SQLFetch(SQLHSTMT StatementHandle) {
	SQLRETURN rc = ::SQLFetch(StatementHandle);
	std::stringstream ss;
	ss << "SQLFetch return " << rc;
	return rc;
}
}

char config_file_name[256];

int import_file(const std::string& import_file_name, const std::string& symbol,
		SQLHDBC hDbc) { // return number of lines successfully parsed.
	std::ifstream file(import_file_name.c_str());
	if (file.bad())
		throw_error("failed to open %s", import_file_name.c_str());
	std::string str;
	std::array<char, 128> databuf;
	std::array<char, 1024> stmtbuf;
	std::stringstream ss;
	int linecnt = 0, skipcnt = 0;
	int year, mon, day, cnt;
	using namespace boost::gregorian;
	date today = day_clock::local_day();
	int maxyear = today.year() + 1;
	SQLRETURN rc = SQL_SUCCESS;
	while (std::getline(file, str)) {
		// 2015-01-16,500.01,508.19,500.00,508.08,2286900,508.08
		cnt = sscanf(str.c_str(), "%d-%d-%d,%s", &year, &mon, &day,
				databuf.data());
		if (cnt != 4 || mon > 12 || mon < 1 || day < 1 || day > 31
				|| year < 1900 || year > maxyear) {
			skipcnt++;
			std::cout << __FUNCTION__ << " skip line " << str << std::endl;
			continue;
		}
		// SELECT importdata('GOOG', '20150118', 500.01,    508.19,    500,508.08,2286900,508.08);
		ss << "SELECT importdata('" << symbol << "', '" << year << std::setw(2)
				<< std::setfill('0') << mon << std::setw(2) << std::setfill('0')
				<< day << "'," << databuf.data() << ");";
		linecnt++;
		if (linecnt < 4)
			std::cout << ss.str() << std::endl;
		if ((linecnt + 1) % 64 == 0) {
			exec_stmt(hDbc, ss.str());
			ss.str("");
		}
	}
	exec_stmt(hDbc, ss.str());
	ss.str("");
	std::cout << __FUNCTION__ << " imported " << linecnt << " lines, skipped "
			<< skipcnt << " lines.\n";
	return linecnt;
}

void init_term()
{
//		typedef boost::log::sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
//    // create sink to stdout
//    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
//    sink->locked_backend()->add_stream(
//        boost::shared_ptr<std::ostream>(&std::cout, boost::empty_deleter()));
//    // flush
//    sink->locked_backend()->auto_flush(true);
//    // format sink
//    sink->set_formatter
//    (
//        /// TODO add your desired formatting
//    );
//    // filter
//    // TODO add any filters
//
//    // register sink
//    boost::log::core::get()->add_sink(sink);
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
	init_term();
}

std::string to_string(const std::chrono::system_clock::time_point& now) {
	std::stringstream ss;
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm local_tm = *localtime(&tt);
	std::chrono::system_clock::time_point secclock =
			std::chrono::system_clock::from_time_t(tt);
	std::chrono::milliseconds msec = std::chrono::duration_cast
			< std::chrono::milliseconds > (now - secclock);

	ss << local_tm.tm_year + 1900 << std::setw(2) << std::setfill('0')
			<< local_tm.tm_mon + 1 << std::setw(2) << std::setfill('0')
			<< local_tm.tm_mday << std::setw(2) << std::setfill('0')
			<< local_tm.tm_hour << std::setw(2) << std::setfill('0')
			<< local_tm.tm_min << std::setw(2) << std::setfill('0')
			<< local_tm.tm_sec << std::setw(2) << std::setfill('0')
			<< msec.count();
	return ss.str();
}


int main_func(int argc, char* argv[]) {
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")("config_file_name,c",
			po::value<std::string>(), "set path of configuration file")(
			"import_file_name,i", po::value<std::string>(),
			"set path of import file")("symbol,s", po::value<std::string>(),
			"set symbol of import file")("log_level,l",
			po::value<std::string>(), "set log level");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		BOOST_LOG_TRIVIAL(info)<< desc << "\n";
		return 1;
	}

	if (vm.count("config_file_name") < 1) {
		std::cout << desc << "\n";
		return 1;
	}
	std::chrono::system_clock::time_point now =
			std::chrono::system_clock::now();

	{
		std::stringstream ss;
//   boost::posix_time::ptime t1 = boost::posix_time::second_clock::local_time();
//     ss << argv[0]<<"-"<<t1.date().year() 
//       << std::setw(2) << std::setfill('0') << t1.date().month()
//       << std::setw(2) << std::setfill('0') << t1.date().day()
//       << std::setw(2) << std::setfill('0') << t1.time_of_day().hours()
//       << std::setw(2) << std::setfill('0') << t1.time_of_day().minutes()
//       << std::setw(2) << std::setfill('0') << t1.time_of_day().seconds()
//       << std::setw(4) << std::setfill('0') << getpid()
//       << ".log";    
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

	BOOST_LOG_TRIVIAL(info)<< argv[0] << " pid " << getpid() << " start at " << to_string(now);

	strcpy(config_file_name, vm["config_file_name"].as<std::string>().c_str());

	char szInConnStr[512], szOutConnStr[2048];
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(config_file_name, pt);

	std::stringstream ss;
// http://docs.adaptivecomputing.com/mwm/Content/topics/databases/postgreSql.html
	ss << "Driver=" << pt.get<std::string>("database.Driver") << ";Servername="
			<< pt.get<std::string>("database.Servername") << ";Port="
			<< pt.get<std::string>("database.Port") << ";Database="
			<< pt.get<std::string>("database.Database") << ";UserName="
			<< pt.get<std::string>("database.UserName") << ";Password="
			<< pt.get<std::string>("database.Password") << ";";
	strcpy(szInConnStr, ss.str().c_str());

	boost::timer::auto_cpu_timer report_cpu_time;

	SQLRETURN rc = SQL_SUCCESS;
	SQLHENV hEnv = NULL;
	rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	boost::shared_ptr<void> envholder(hEnv, odbc_deleter(SQL_HANDLE_ENV));
	if (ODBC_FAILED(rc)) {
		throw_error("SQLAllocHandle return %d", rc);
	}
	rc = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3,
			SQL_IS_INTEGER);
	if (ODBC_FAILED(rc)) {
		throw_error("SQLSetEnvAttr return %d", rc);
	}
	SQLHDBC hDbc;
	rc = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	boost::shared_ptr<void> dbcholder(hDbc, odbc_deleter(SQL_HANDLE_DBC));
	if (ODBC_FAILED(rc)) {
		throw_error("SQLAllocHandle return %d", rc);
	}

	SQLSMALLINT dwOutConnStrLen = sizeof(szOutConnStr);
	rc = SQLDriverConnect(hDbc, NULL, reinterpret_cast<SQLCHAR*>(szInConnStr),
			strlen(szInConnStr), reinterpret_cast<SQLCHAR*>(szOutConnStr),
			dwOutConnStrLen, &dwOutConnStrLen, SQL_DRIVER_COMPLETE //SQL_DRIVER_PROMPT // SQL_DRIVER_NOPROMPT
			);
	if (ODBC_FAILED(rc)) {
		extract_error("SQLDriverConnect", hDbc, SQL_HANDLE_DBC);
		throw_error("SQLDriverConnect return %d, connection string: %s", rc,
				szInConnStr);
	}
	BOOST_LOG_TRIVIAL(debug)<< szInConnStr << " return " << rc << std::endl;
	if (dwOutConnStrLen > 0) {
		BOOST_LOG_TRIVIAL(debug)<< "Out connection string: " << szOutConnStr << std::endl;
	}

	if (vm.count("import_file_name")) {
		if (vm.count("symbol")) {
			import_file(vm["import_file_name"].as<std::string>(),
					vm["symbol"].as<std::string>(), hDbc);
		} else {
			throw_error("Missing symbol when import_file_name is specified");
		}
	}
//	exec_sql_query(hDbc,
//			"SELECT symbol,tdate, open FROM daily where tdate >= '2015-01-01'::date ORDER BY symbol, tdate limit 2;");

	analyze_open_trend(hDbc);

	SQLDisconnect(hDbc);
	BOOST_LOG_TRIVIAL(info)<< argv[0] << " pid " <<std::dec << getpid() << " done at "
	<< to_string(std::chrono::system_clock::now());
	return 0;
}

static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
                         void* context,
                         bool succeeded)
{
  printf("Dump path: %s\n", descriptor.path());
  return succeeded;
}

int main(int argc, char* argv[]) {
	int ret = -1;
	  google_breakpad::MinidumpDescriptor descriptor("/tmp");
	  google_breakpad::ExceptionHandler eh(descriptor,
	                                       NULL,
	                                       dumpCallback,
	                                       NULL,
	                                       true,
	                                       -1);

	try {
		ret = main_func(argc, argv);
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error)<< argv[0] << " exception: " << e.what() << std::endl;
	}
	return ret;
}
