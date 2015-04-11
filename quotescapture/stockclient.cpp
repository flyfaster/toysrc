// stockclient.cpp : Defines the entry point for the console application.
//
//
// Copyright (c) 2008-2008 Onega Zhang (onega at hotmail dot com)
//
// Distributed under the MIT License
//

#include "stdafx.h"

namespace po = boost::program_options;
using boost::asio::ip::tcp;
#include <wx/wx.h>
#include "MainAppApis.h"

boost::function0<void> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
	switch (ctrl_type)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		console_ctrl_function();
		sleep();
		return TRUE;
	default:
		return FALSE;
	}
}


int main(int argc, char* argv[])
{

	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("symbolfile", po::value<std::string>(), "set path of symbol file")
			;
		po::variables_map vm;        
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);   
		if (vm.count("help")) {
			//cout << desc << "\n";
			return 1;
		}
		std::string input_file = "stock_symbols.txt";
		if (vm.count("symbolfile")) {
			input_file = vm["symbolfile"].as<std::string>();

		} else {
			//cout << "symbolfile was not set.\n";
		}
		//cout << "load symbols from " 
		//	<< input_file << std::endl;
		//SingleSymbolCol::Instance().load(input_file);
	}
	catch(std::exception& e) {
		wxLogError(wxT("%s exception %s"), wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(e.what()));
		return 1;
	}
	catch(...) {
		//cerr << "Exception of unknown type!\n";
	}
	try
	{
		for (int i=0; i<2; i++)
		{
			//requester.create_connection();
		}
		//console_ctrl_function = boost::bind(&boost::asio::io_service::stop, &io_service);
		//SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
		//io_service.run();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}

	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR    lpCmdLine,
					 int       nCmdShow)
{
	MainApp_RunApp();
	return 0;
}
