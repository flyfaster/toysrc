#include "stdafx.h"
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/log.h>

#include <boost/asio.hpp>
#include <boost/date_time.hpp>
using boost::asio::ip::tcp;
#include "stock_symbol.h"
#include "stock_symbol_collection.h"
#include "http_client_factory.h"
#include "download_thread.h"
#include "MainApp.h"
//extern http_client_factory requester;

struct impl_download_thread
{
	boost::mutex mut;
	boost::condition_variable_any cond;

};
long download_thread::run()
{
	wxLogVerbose(wxT("%s started"), wxString::FromAscii(__FUNCTION__));
	//MainApp::Idle(100, &mut, &cond);
	boost::posix_time::ptime start_time = boost::posix_time::second_clock::local_time();
	bool keep_waiting = false;
	std::string current_symbol;
	wxGetApp().GetHttpClientFactory()->add_http_event_listener( (ihttp_event_p)this);
	while(MainApp::Stopping()==false)
	{
		while(MainApp::Stopping())
		{
			current_symbol = SingleSymbolCol::Instance().pop_symbol();
			if (current_symbol.length())
			{
				wxLogVerbose(wxT("%s process symbol: %s"), wxString::FromAscii(__FUNCTION__),
					wxString::FromAscii(current_symbol.c_str()));
				//requester.prepare_nasdaq_request(current_symbol);
				wxGetApp().GetHttpClientFactory()->add_task(current_symbol, 1);
			}
			else
			{
				wxLogVerbose(wxT("%s no more symbol"), wxString::FromAscii(__FUNCTION__));
				break;
			}
		}
		wxGetApp().GetHttpClientFactory()->check_dead_tasks();
		while (wxGetApp().GetHttpClientFactory()->active_connection_count()<10 
            && wxGetApp().GetHttpClientFactory()->task_queue_size())
		{
			wxGetApp().GetHttpClientFactory()->start_task();
		}
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += 5;
		boost::unique_lock<boost::mutex> lock(d->mut);
		d->cond.timed_wait(lock, xt);
	}
	wxLogVerbose(wxT("%s stopped"), wxString::FromAscii(__FUNCTION__));
	return 0;
}

void download_thread::stop()
{
	notify();
}

void download_thread::notify()
{// wakeup this thread
	d->cond.notify_all();
}

download_thread::download_thread()
{
d = new impl_download_thread;
}

void download_thread::operator()()
{
	run();
}

download_thread::~download_thread()
{
	delete d;
	d=NULL;
}

void download_thread::on_complete( ihttp_client_p client, boost::system::error_code err )
{
	notify();
}

void download_thread::dump()
{

}