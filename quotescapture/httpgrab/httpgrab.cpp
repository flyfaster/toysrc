// httpgrab.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/enable_shared_from_this.hpp>
using boost::asio::ip::tcp;
#include <Windows.h>

#include "stringutils.h"
#include "async_http_client.h"
struct application_args
{
    std::string url;
    std::string local_file_path;
    unsigned long timeout_ms;
};
int _tmain(int argc, _TCHAR* argv[])
{
    using namespace boost::program_options;
    boost::program_options::variables_map vm;
    options_description options("Allowed options");
    options.add_options()
        ("help,h", "print help messages")
        ("url,u", boost::program_options::value<std::string>(),"url link to download")
        ("localfilepath,f", boost::program_options::value<std::string>(), "file path to save on local disk")
        ("timeoutms,t", boost::program_options::value<unsigned long>(), "number of milliseconds the download operation would timeout")
        ;
    try
    {
        std::vector<std::string> args = boost::program_options::split_winmain(to_string(GetCommandLine()));
        boost::program_options::store(command_line_parser(args).options(options).run(),vm);
        //boost::program_options::store(boost::program_options::parse_command_line(__argc, __argv, options), vm);
        boost::program_options::notify(vm);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        std::cout << options << std::endl;
        return __LINE__;
    }
    application_args myargs;
    myargs.timeout_ms = 0;
    if (vm.count("url"))
        myargs.url = vm["url"].as<std::string>();
    if (vm.count("localfilepath"))
        myargs.local_file_path=vm["localfilepath"].as<std::string>();
    if (vm.count("timeoutms"))
        myargs.timeout_ms = vm["timeoutms"].as<unsigned long>();
    if (vm.count("help") || argc == 1 || myargs.local_file_path.length()==0 || myargs.url.length()==0)
    {
        std::cout << options << std::endl;
        return 0;
    }
    try
    {
        boost::asio::io_service io_service;
        boost::shared_ptr<async_http_client> c=async_http_client::create (io_service, myargs.url, myargs.local_file_path, myargs.timeout_ms);
        if (c->is_valid()==false)
        {
            std::cout << options << std::endl;
            return __LINE__;
        }
        c->do_resolve();
        io_service.run();
        if (c->get_error())
        {
            return c->get_error().value();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
    std::cout << "Program completed\n";
	return 0;
}

