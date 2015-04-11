/*
Created by Onega Zhang
reference - http://bbs.dameng.com/redirect.php?tid=1116&goto=lastpost
*/
#include <boost/program_options.hpp>
#include "odbcexportapp.h"
#include "mainwnd.h"
#include <iostream>
#include "strtypes.h"

namespace po = boost::program_options;
IMPLEMENT_APP(ODBCExportApp)
ODBCExportApp::ODBCExportApp() {}
bool ODBCExportApp::OnInit() 
{
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("dsn", po::value<std::string>(), "DSN or connection string")
    ("table", po::value<std::string>(), "Database table name")
    ("file", po::value<std::string>(), "export/import table data file path")
    ;
    
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help") )
        {
//             print_usage(argv[0], desc);
            return __LINE__;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what()<<std::endl;
//         print_usage(argv[0], desc);
        return __LINE__;
    }  
    
  wxFrame *the_frame = new MainWnd();//(NULL, ID_MYFRAME, argv[0]);
  the_frame->Show(true);
  SetTopWindow(the_frame);
  std::cout << "size of wxChar is " << sizeof(wxChar) 
	    << ", sizeof (wchar_t) is " << sizeof(wchar_t) 
	    << ", sizeof(my_tchar) " << sizeof(my_tchar)
	    << ", sizeof(SQLTCHAR) " << sizeof(SQLTCHAR)
	    << ", sizeof(SQLCHAR) " << sizeof(SQLCHAR)
	    << std::endl;
  return true;
}
