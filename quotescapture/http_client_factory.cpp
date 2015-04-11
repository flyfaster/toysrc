#include "StdAfx.h"
#include <wx/log.h>
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include <boost/filesystem.hpp>
using boost::asio::ip::tcp;
#include "stock_symbol.h"
#include "http_client_factory.h"
#include "stock_symbol_collection.h"
#include <wx/app.h>
#include "MainApp.h"


struct impl_http_client_factory
{
	std::map<std::string, ihttp_client_ptr> clients;
	std::map<std::string, std::string> download_tasks;
	ihttp_event_p m_listener;
	boost::mutex thread_locker ;
};
http_client_factory::http_client_factory(void)
{
	d = new impl_http_client_factory;
	d->m_listener = NULL;
}

http_client_factory::~http_client_factory(void)
{
	delete d;
	d = NULL;
}

void http_client_factory::on_complete(ihttp_client_p client, boost::system::error_code err ) 
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	wxLogVerbose( wxT("%s %s download complete"), wxString::FromAscii(__FUNCTION__), 
		wxString::FromAscii(client->key().c_str()));
	if (d->m_listener)
	{
		d->m_listener->on_complete(client, err);

	}
	d->clients.erase(client->key());
	start_task(); // download next page
}

extern boost::asio::io_service io_service;
void http_client_factory::prepare_google_request( const std::string& stock_symbol )
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	using namespace boost::posix_time;
	ptime local_now = second_clock::local_time();
	typedef boost::date_time::local_adjustor<ptime, -8, us_dst> us_eastern;
	ptime ny_now =  us_eastern::local_to_utc(local_now);
	std::stringstream ss;
	ss << stock_symbol 
		<< std::setw(4) << std::setfill('0') << ny_now.date().year()
		<< std::setw(2) << std::setfill('0') << (int)ny_now.date().month()
		<< std::setw(2) << std::setfill('0') << ny_now.date().day()
		<< std::setw(2) << std::setfill('0') << ny_now.time_of_day().hours()
		<< std::setw(2) << std::setfill('0') << ny_now.time_of_day().minutes()
		<< ".htm"
		;
	std::string filename = ss.str();
	//cout << __FUNCTION__ << " (" << stock_symbol <<") save as " << filename <<std::endl;
	if (stock_symbol.size())
	{
		//ihttp_client_ptr client=http_client::create(io_service, 
		//	"finance.google.com",
		//	std::string("/finance?q=") + stock_symbol,
		//	filename );
		//client->add_event_listener(this);
		//clients[client->key()] = client;
	}

}
void http_client_factory::create_connection(void)
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	std::string stock_symbol = SingleSymbolCol::Instance().pop_symbol();
	prepare_nasdaq_request(stock_symbol);

}

void http_client_factory::prepare_nasdaq_request( const std::string& stock_symbol )
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
// http://www.nasdaq.com/aspx/nlstrades.aspx?symbol=NOK&selected=NOK
	if (stock_symbol.length()<1)
	{
		wxLogError( wxT("%s input is null"), wxString::FromAscii(__FUNCTION__));
		return;
	}
	char url_buf[1024] = {0};
	sprintf_s(url_buf, sizeof(url_buf), 
		"http://www.nasdaq.com/aspx/nlstrades.aspx?symbol=%s&selected=%s",
		stock_symbol.c_str(), stock_symbol.c_str());

	char filename_buf[MAX_PATH+1024]={0};
    build_file_name(filename_buf, sizeof(filename_buf), stock_symbol);
    ihttp_client_ptr client=http_client::create_http1(io_service, 
		"www.nasdaq.com",
		url_buf,
		filename_buf );
    if (client)
    {
        boost::shared_ptr<http_client_factory> p(this);
        p->self = p;
	    client->add_event_listener(p);
	    d->clients[client->key()] = client;
    }
    else
    {
        wxLogError(wxT("%s Failed to initiate http connection (%s, %s)"), 
            wxString::FromAscii(__FUNCTION__),
            wxString::FromAscii(url_buf), 
            wxString::FromAscii(filename_buf));
    }

}
void http_client_factory::prepare_nasdaq_request(
    const std::string& stock_symbol, 
    int page_no)
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	// http://www.nasdaq.com/aspx/nlstrades.aspx?pageno=2&&symbol=NOK&selected=NOK
	if (stock_symbol.length()<1)
	{
		wxLogError( wxT("%s input is null"), wxString::FromAscii(__FUNCTION__));
		return;
	}
	char url_buf[1024] = {0};
	sprintf_s(url_buf, sizeof(url_buf), 
		"http://www.nasdaq.com/aspx/nlstrades.aspx?pageno=%d&&symbol=%s&selected=%s",
		page_no, stock_symbol.c_str(), stock_symbol.c_str());

	char filename_buf[MAX_PATH+1024]={0};
	if (!MainApp::GetAppPath())
	{
		wxLogError(wxT("%s Failed to get application path"), 
            wxString::FromAscii(__FUNCTION__));
		return;
	}
	using namespace boost::posix_time;
	ptime local_now = second_clock::local_time();
	ptime ny_market_time = MainApp::LocalToNYMarketDate(local_now);
	sprintf_s(filename_buf, sizeof(filename_buf), 
		"%s%s %04d%02d%02dpage%03d.txt",
		wx2std(MainApp::GetAppPath()).c_str(),stock_symbol.c_str(), 
		ny_market_time.date().year(),
		ny_market_time.date().month(),
		ny_market_time.date().day(),
		page_no);
	ihttp_client_ptr client=http_client::create_http1(io_service, 
		"www.nasdaq.com",
		url_buf,
		filename_buf );

	client->add_event_listener(shared_from_this());
	d->clients[client->key()] = client;
}

size_t http_client_factory::active_connection_count()
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	return d->clients.size();
}
void http_client_factory::add_task(const std::string& stock_symbol, int page_no)
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	if (stock_symbol.length()<1)
	{
		wxLogError( wxT("%s input is null"), wxString::FromAscii(__FUNCTION__));
		return;
	}
	char url_buf[1024] = {0};
	sprintf_s(url_buf, sizeof(url_buf), 
		"http://www.nasdaq.com/aspx/nlstrades.aspx?pageno=%d&&symbol=%s&selected=%s",
		page_no, stock_symbol.c_str(), stock_symbol.c_str());

	char filename_buf[MAX_PATH+1024]={0};
	if (!MainApp::GetAppPath())
	{
		wxLogError(wxT("%s Failed to get application path"), 
            wxString::FromAscii(__FUNCTION__));
		return;
	}
	using namespace boost::posix_time;
	ptime local_now = second_clock::local_time();
	ptime ny_market_time = MainApp::LocalToNYMarketDate(local_now);
	sprintf_s(filename_buf, sizeof(filename_buf), 
		"%s%s %04d%02d%02dpage%03d.txt",
		wx2std(MainApp::GetAppPath()).c_str(),stock_symbol.c_str(), 
		ny_market_time.date().year(),
		ny_market_time.date().month(),
		ny_market_time.date().day(),
		page_no);
	d->download_tasks[url_buf] = filename_buf;
}

bool http_client_factory::start_task()
{
	bool ret = false;
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	if (d->download_tasks.size())
	{
		std::map<std::string, std::string>::iterator it=d->download_tasks.begin();
		std::string url = it->first;
		std::string filename = it->second;
		// if file already exists, we don't download again
		boost::filesystem::path dst_path(filename, boost::filesystem::native);
		if (boost::filesystem::exists(dst_path))
		{
			wxLogVerbose(wxT("%s %s already exists."), 
                wxString::FromAscii(__FUNCTION__), 
				wxString::FromAscii(filename.c_str()));
		}
		else
		{
			ihttp_client_ptr client=http_client::create_http1(io_service, 
				"www.nasdaq.com",
				url,
				filename );
			client->add_event_listener(shared_from_this());
			d->clients[client->key()] = client;
			ret = true;
		}
		d->download_tasks.erase(it);
		return ret;
	}
	return false;
}

size_t http_client_factory::task_queue_size()
{
	return d->download_tasks.size();
}

void http_client_factory::add_total_task( const std::string& symbol, 
                                         int total_page_no )
{
	//boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	for (int i=2; i<=total_page_no; i++)
	{
		add_task(symbol, i);
	}
}

void http_client_factory::add_http_event_listener( ihttp_event_p plistener )
{
	d->m_listener = plistener;
}

void http_client_factory::dump()
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	std::basic_stringstream<TCHAR> tss;
	tss << wxT("download task queue size is ") << d->download_tasks.size() << std::endl;
	for (std::map<std::string, ihttp_client_ptr>::const_iterator it=d->clients.begin();
		it!=d->clients.end();
		it++)
	{
		tss << _T("downloading ") << wxString::FromAscii(it->first.c_str()) 
            << _T(" using ") << it->second->elapsed_ms() << _T("ms")<< std::endl;
	}
	tss << _T("listener is ") << std::hex << d->m_listener << std::endl;
	wxLogVerbose(tss.str().c_str());
}

void http_client_factory::check_dead_tasks()
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	const int MAX_TASK_DURATION_MS = 1000 * 30; // kill a task if it takes longer than 30 seconds
	for (std::map<std::string, ihttp_client_ptr>::const_iterator it=d->clients.begin();
		it!=d->clients.end();
		it++)
	{
		ihttp_client_ptr pclient = it->second;
		int elapsed_time = pclient->elapsed_ms();
		if (elapsed_time>= MAX_TASK_DURATION_MS)
		{
			// kill it
			std::string url = pclient->get_url();
			std::string fn = pclient->get_filename();
			pclient->close();
			d->clients.erase(it->first);
			d->download_tasks[url] = fn;
			wxLogVerbose(_T("%s retry %s later due to timeout %dms."), 
                wxString::FromAscii(__FUNCTION__), 
				wxString::FromAscii(url.c_str()), elapsed_time);
		}
	}

}

void http_client_factory::build_file_name( char* filename_buf, 
                                          int filename_buf_length,
                                          const std::string &stock_symbol )
{
    using namespace boost::posix_time;
    ptime local_now = second_clock::local_time();
    ptime ny_market_time = MainApp::LocalToNYMarketDate(local_now);
    sprintf_s(filename_buf, filename_buf_length, 
        "%s%s %04d%02d%02dpage001.txt",
        wx2std(MainApp::GetAppPath()).c_str(),stock_symbol.c_str(), 
        ny_market_time.date().year(),
        ny_market_time.date().month(),
        ny_market_time.date().day() );
}

boost::shared_ptr<http_client_factory> http_client_factory::shared_from_this()
{
    return boost::static_pointer_cast<http_client_factory>(ihttp_event::shared_from_this());

}
