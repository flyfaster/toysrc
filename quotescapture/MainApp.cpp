#include "StdAfx.h"
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <loki/Singleton.h>
#include <loki/pimpl.h>

using boost::asio::ip::tcp;
#include "SqliteSelectCallback.h"
#include "StockDB.h"
#include "stock_symbol_collection.h"
#include "http_client.h"
#include "http_client_factory.h"
#include "VerboseDialog.h"
#include "MainFrame.h"
#include "download_thread.h"
#include "MainApp.h"
#include "MainAppApis.h"
boost::asio::io_service io_service;

void STOCKCLIENT_API sleep(int sleep_seconds)
{
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += sleep_seconds;
    boost::thread::sleep(xt);
}

int STOCKCLIENT_API MainApp_RunApp()
{
    MainApp::RunApp();
    return 0;
}

//IMPLEMENT_APP_NO_MAIN(MainApp)
struct PrivateMainApp
{
    download_thread downloader;
	boost::program_options::variables_map po_vm; 
	static bool m_stop;
	boost::shared_ptr<boost::thread> download_worker;
    boost::shared_ptr<boost::asio::io_service::work> m_io_dummy_work;
    http_client_factory requester;
    void stop();
};

void PrivateMainApp::stop()
{
    downloader.stop();
    //downloader.join();
    if (download_worker)
        download_worker->join();
    if (!m_stop)
        io_service.stop();
    sleep(1);
    m_stop = true;
}

static wxAppConsole *wxCreateApp()
{
	wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,"my app");
	return new MainApp;
}

MainApp& wxGetApp()
{
    return *((MainApp*)wxApp::GetInstance());
}

// the only public function, declared also in library's header
void MainApp::RunApp()
{
	wxApp::SetInitializerFunction(wxCreateApp);
	wxEntry(0,NULL);
}

MainApp::MainApp(void)
{
	m_data = new PrivateMainApp;
    m_data->m_io_dummy_work.reset(new boost::asio::io_service::work(io_service));
}

MainApp::~MainApp(void)
{
	SingleStockDB::Instance().Close();
    delete m_data;
}

namespace po = boost::program_options;

bool MainApp::OnInit(void)
{
	VerboseDialog::Instance()->Show(true);
	wxLogMessage( wxString::FromAscii(__FUNCTION__));
	//return true;
	StartTcpService();
	//ParseCommandLine();
	//SingleStockDB::Instance().Open();
	wxFrame* main_wnd = NULL;
	main_wnd = MainFrame::Instance().GetFrame();
	main_wnd->Show(true);
	SetTopWindow(main_wnd);
	// process a symbol
	std::string stock_symbol="ADSK";
	//requester.prepare_nasdaq_request(stock_symbol);
	//Idle(100);
	//download_worker.reset(new boost::thread(boost::bind(&download_thread::run, &downloader)));
	return true;
}

void MainApp::ParseCommandLine()
{
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("symbolfile", po::value<std::string>(), "set path of symbol file")
			;
		std::vector<std::string> args = po::split_winmain(GetCommandLineA());
		po::store(po::command_line_parser(args).options(desc).run(), m_data->po_vm);
		//po::store(po::parse_command_line(__argc, __argv, desc), po_vm);
		po::notify(m_data->po_vm);   
		if (m_data->po_vm.count("help")) {
			//cout << desc << "\n";
		}
		std::string input_file = "stock_symbols.txt";
		if (m_data->po_vm.count("symbolfile")) {
			input_file = m_data->po_vm["symbolfile"].as<std::string>();
		} else {
			wxLogVerbose(wxT("symbolfile was not set.\n"));
		}
		wxLogInfo(wxT("%s load file %s"), wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(input_file.c_str()));
		SingleSymbolCol::Instance().load(input_file);
	}
	catch(std::exception& e) {
		wxLogError(wxT("%s error: %s"), wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(e.what()));
	}
	catch(...) {
		wxLogError(wxT("%s unknown exception."), wxString::FromAscii(__FUNCTION__));
	}
}

void MainApp::StartTcpService()
{
	wxLogVerbose(wxString::FromAscii(__FUNCTION__));
	boost::thread(boost::bind(&boost::asio::io_service::run, &io_service)); 
}

void MainApp::StopTcpService()
{
	wxLogVerbose(wxString::FromAscii(__FUNCTION__));
    if (m_data)
        m_data->stop();
}

bool MainApp::Stopping()
{
	return PrivateMainApp::m_stop;
}
bool PrivateMainApp::m_stop;
int MainApp::OnExit()
{
	StopTcpService();
	if (VerboseDialog::Instance())
	{
		VerboseDialog::Instance()->Show(false);
		VerboseDialog::Instance()->Destroy();			
	}
	return 0;
}

boost::posix_time::ptime MainApp::LocalToNY(const boost::posix_time::ptime& shanghai_ptime )
{
	//DST starts on Sunday, March 8, 2009 at 2:00 AM local standard time
	//DST ends on Sunday, November 1, 2009 at 2:00 AM local daylight time
	using namespace boost::posix_time;
	ptime ny_now = shanghai_ptime;
	boost::gregorian::date ny_start_date(ny_now.date().year(), boost::date_time::Mar,1 /*boost::date_time::Sunday*/);
	// find second sunday
	ny_start_date += boost::gregorian::days(7 + (7 - ny_start_date.day_of_week())%7);
	boost::gregorian::date ny_end_date(ny_now.date().year(), boost::date_time::Nov, 1 /*boost::date_time::Sunday*/);
	// find first sunday
	ny_start_date += boost::gregorian::days((7 - ny_start_date.day_of_week())%7);
	
	ptime ny_dst_start(ny_start_date, hours(2));
	ptime ny_dst_end(ny_end_date, hours(1));
	
	if (ny_now >= ny_dst_start && ny_now < ny_dst_end)
		ny_now -= hours(12);
	else
		ny_now -= hours(13);
	return ny_now;
}

boost::posix_time::ptime MainApp::LocalToNYMarketDate( const boost::posix_time::ptime& shanghai_ptime )
{
	using boost::gregorian::days;
	using boost::posix_time::hours;
	using boost::posix_time::minutes;
	boost::posix_time::ptime ny_ptime = LocalToNY(shanghai_ptime); 
	return NYToMarketDate(ny_ptime);
}

boost::posix_time::ptime MainApp::NYToMarketDate( const boost::posix_time::ptime& ny_ptime_input )
{
	using boost::posix_time::ptime;
	using boost::gregorian::days;
	using boost::posix_time::hours;
	using boost::posix_time::minutes;
	// market open at 9:30AM, close at 16:00 PM
	ptime ny_ptime = ny_ptime_input; 
	ptime market_open_time(ny_ptime.date(), hours(9)+minutes(30));
	ptime market_close_time(ny_ptime.date(), hours(16));
	switch(ny_ptime.date().day_of_week())
	{
	case boost::date_time::Monday:
		if (ny_ptime>=market_open_time)
			return ny_ptime;
		else
		{
			ny_ptime -= days(3); // last Friday
			return ny_ptime;
		}
		break;
	case boost::date_time::Tuesday:
	case boost::date_time::Wednesday:
	case boost::date_time::Thursday:
	case boost::date_time::Friday:
		if (ny_ptime>=market_open_time)
			return ny_ptime;
		else
		{
			ny_ptime -= days(1);
			return ny_ptime;
		}
		break;
	case boost::date_time::Saturday:
		ny_ptime -= days(1); // last Friday
		break;
	case boost::date_time::Sunday:
		ny_ptime -= days(2); // last Friday
		break;
	}
	return ny_ptime;

}

TCHAR module_path_buf[MAX_PATH+1024]={0};
LPCTSTR MainApp::GetAppPath()
{
	if (module_path_buf[0])
		return module_path_buf;
	GetModuleFileName(NULL, module_path_buf, sizeof(module_path_buf)/sizeof*module_path_buf-1);
	TCHAR *last_back_slash_position = _tcsrchr(module_path_buf, wxT('\\'));
	if (!last_back_slash_position)
	{
		wxLogError(wxT("%s invalid module path name %s"), wxString::FromAscii(__FUNCTION__),
			module_path_buf);
		return module_path_buf;
	}
	*(last_back_slash_position+1)=0;
	return module_path_buf;
}

int MainApp::FilterEvent( wxEvent& event )
{
	if ( event.GetEventType()==wxEVT_KEY_DOWN )
	{
		int key_code = ((wxKeyEvent&)event).GetKeyCode();
		if (key_code == WXK_F9 || key_code == VK_F9)
		{
			wxLogVerbose(wxT("%s %d"), wxString::FromAscii(__FUNCTION__), __LINE__);
			return true;
		}
	}
	//wxLogVerbose(wxT("%s %d"), wxString::FromAscii(__FUNCTION__), __LINE__);
	return -1;
}

boost::posix_time::ptime MainApp::NYToLocal( const boost::posix_time::ptime& ny_now )
{
	//DST starts on Sunday, March 8, 2009 at 2:00 AM local standard time
	//DST ends on Sunday, November 1, 2009 at 2:00 AM local daylight time
	using namespace boost::posix_time;
	ptime shanghai_ptime;
	boost::gregorian::date ny_start_date(ny_now.date().year(), boost::date_time::Mar,1 /*boost::date_time::Sunday*/);
	// find second sunday
	ny_start_date += boost::gregorian::days(7 + (7 - ny_start_date.day_of_week())%7);
	boost::gregorian::date ny_end_date(ny_now.date().year(), boost::date_time::Nov, 1 /*boost::date_time::Sunday*/);
	// find first sunday
	ny_start_date += boost::gregorian::days((7 - ny_start_date.day_of_week())%7);

	ptime ny_dst_start(ny_start_date, hours(2));
	ptime ny_dst_end(ny_end_date, hours(1));

	if (ny_now >= ny_dst_start && ny_now < ny_dst_end)
		shanghai_ptime = ny_now + hours(12);
	else
		shanghai_ptime = ny_now + hours(13);
	return shanghai_ptime;

}

http_client_factory* MainApp::GetHttpClientFactory()
{
	return &(m_data->requester);
}

void MainApp::Dump()
{
	m_data->downloader.dump();
	m_data->requester.dump();
	SingleSymbolCol::Instance().dump();
}

std::string wx2std(wxString s){
	std::string s2;
	if(s.wxString::IsAscii()) {
		s2=s.wxString::ToAscii();
	} else {
		const wxWX2MBbuf tmp_buf = wxConvCurrent->cWX2MB(s);
		const char *tmp_str = (const char*) tmp_buf;
		s2=std::string(tmp_str, strlen(tmp_str));
	}
	return s2;
} 
