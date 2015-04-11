#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time.hpp>
#include <sstream>
#include <locale>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "src/column.h"
#include <vector>
#include "src/odbc_helper.h"
#include "src/itable_archive.h"
#include "src/otable_archive.h"
namespace po = boost::program_options;
void print_usage(const char* app_name,   po::options_description& desc)
{
    std::cout << desc << "\n";
    std::cout << app_name << " --dsn=\"DSN=psqlODBC;\"  --table prices" << std::endl;
    std::cout << app_name << " usage:" << std::endl;
    std::cout << app_name << " \"DSN=<ODBC data source name>;\" " << std::endl;
    std::cout << app_name << " \"<ODBC connection string>;\" " << std::endl;
    std::cout << "sample usage:" << std::endl
              << app_name << " \"Driver={PostgreSQLW};DATABASE=testdb;SERVER=localhost;PORT=5432;UID=postgres;PWD=******;\" "
              << std::endl
              << app_name << " \"DSN=psqlODBC;\" "
              << std::endl;
    std::cout << "psqlODBC is defined in /etc/odbc.ini"<<std::endl;
    std::cout << "PostgreSQLW is defined in /etc/odbcinst.ini" << std::endl;
    std::cout << "Windows XP file postion: c:\\windows\\odbc.ini c:\\windows\\odbcinst.ini.\n";
    std::cout << "Find connection string from http://www.connectionstrings.com/postgre-sql" << std::endl;
}
