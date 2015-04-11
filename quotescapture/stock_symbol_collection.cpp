#include "StdAfx.h"
#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <wx/log.h>
#include "stock_symbol_collection.h"
#include "stock_symbol.h"
#include "StockDB.h"

//#define USE_LOKI_PIMPL

typedef std::map<std::string, ptr_stock_symbol> symbols_collection_type;
std::vector<std::string> stock_symbols_;
boost::mutex mut;
#ifndef USE_LOKI_PIMPL
struct impl_stock_symbol_collection 
#else
template<> struct Loki::ImplOf<stock_symbol_collection>
#endif
{
	size_t m_current_index;
	symbols_collection_type symbols;
	void init();
	boost::mutex thread_locker ;
};

#ifdef USE_LOKI_PIMPL
typedef  Loki::ImplOf<stock_symbol_collection> impl_stock_symbol_collection;
#endif

void impl_stock_symbol_collection::init()
{
	m_current_index = 0;
}

stock_symbol_collection::stock_symbol_collection(void)
{
#ifndef USE_LOKI_PIMPL
	d = new impl_stock_symbol_collection; 
#endif

	d->init();

}

stock_symbol_collection::~stock_symbol_collection(void)
{
#ifndef USE_LOKI_PIMPL
	delete d;
	d = NULL;
#endif
}

// load list from text file, one stock symbol per line
int stock_symbol_collection::load(const std::string& filename)
{
	std::ifstream input_file;
	input_file.open(filename.c_str());
	if (input_file.bad())
	{
		wxLogError(wxT("%s open %s error"), wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(filename.c_str()));
		return 0;
	}
	std::string one_symbol;
	while(!input_file.eof())
	{
		one_symbol.clear();
		std::getline(input_file, one_symbol);
		if (one_symbol.length())
			stock_symbols_.push_back(one_symbol);
	}
	input_file.close();
	wxLogMessage(wxT("%s loaded %d symbols from %s"), wxString::FromAscii(__FUNCTION__),
		stock_symbols_.size(), wxString::FromAscii(filename.c_str()));
	return 0;
}

std::string stock_symbol_collection::pop_symbol()
{
	if (stock_symbols_.size()==0 || d->m_current_index >= stock_symbols_.size())
		return "";

	//m_current_index = m_current_index % stock_symbols_.size();
	return stock_symbols_[d->m_current_index++];
	//if (stock_symbols_.size())
	//{
	//	std::string one_symbol = stock_symbols_.front();
	//	stock_symbols_.pop_front();
	//	return one_symbol;
	//}
	return "";
}

void stock_symbol_collection::push( const std::string& symbol )
{
	stock_symbols_.push_back(symbol);
}

bool stock_symbol_collection::is_end()
{
	return d->m_current_index >= stock_symbols_.size();
}


void stock_symbol_collection::add( const std::string& symbol, const std::string& quote_content )
{
	boost::unique_lock<boost::mutex> lock(mut);
	// TBD
	// parse local time and price
	std::string price;
	std::string price_time;
	const std::string price_token_prefix ="_l\">";
	const std::string price_token_postfix = "</span>";
	const std::string localtime_prefix="_ltt\">";
	const std::string localtime_postfix = "</span>";
	GetMidString(quote_content, price_token_prefix, price_token_postfix, price);
	GetMidString(quote_content, localtime_prefix, localtime_postfix, price_time);
	//if (price.length()&&price_time.length()&&price_time.find("Close")==std::string::npos)
	{
		SingleStockDB::Instance().AddQuote(symbol.c_str(), price_time.c_str(), price.c_str());
	}
	return;
}

void stock_symbol_collection::GetMidString( const std::string &quote_content, 
										   const std::string &price_token_prefix, 
										   const std::string& price_token_postfix, std::string &price )
{
	std::string::size_type prefix_pos = quote_content.find(price_token_prefix);
	std::string::size_type postfix_pos = quote_content.find(price_token_postfix, prefix_pos);
	std::string::size_type not_found = std::string::npos;
	if (prefix_pos!=not_found && postfix_pos!=not_found)
	{
		postfix_pos+=+price_token_prefix.length();
		price = quote_content.substr(prefix_pos, postfix_pos-prefix_pos);
	}
	else
		return;
}

size_t stock_symbol_collection::size()
{
	return stock_symbols_.size();
}

void stock_symbol_collection::add_symbol( const std::string& symbol, int toal_page_no )
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	symbols_collection_type::const_iterator it = d->symbols.find(symbol);
	if (it!=d->symbols.end())
	{
		it->second->total_pages(toal_page_no);
	}
	else
	{
		ptr_stock_symbol psymbol(new stock_symbol());
		psymbol->total_pages(toal_page_no);
		psymbol->symbol(symbol);
		d->symbols[symbol] = psymbol;
	}
}

void stock_symbol_collection::add_symbol(const std::string& symbol)
{
	std::vector<std::string>::iterator it = std::find(stock_symbols_.begin(), stock_symbols_.end(), symbol);
	if (it==stock_symbols_.end())
	{
		stock_symbols_.push_back(symbol);
		wxLogInfo(wxT("%s(%s)"), wxString::FromAscii(__FUNCTION__), wxString::FromAscii(symbol.c_str()));
	}
	else
	{
		wxLogWarning(wxT("%s(%s) already exists."), wxString::FromAscii(__FUNCTION__), wxString::FromAscii(symbol.c_str()));
	}
}
typedef Loki::SingletonHolder<	stock_symbol_collection, 
Loki::CreateUsingNew, 
Loki::PhoenixSingleton> SingleSymbolColHolder;

stock_symbol_collection& stock_symbol_collection::Instance()
{
	return SingleSymbolColHolder::Instance();
}

void stock_symbol_collection::add_symbol_page( const std::string& symbol, int page, bool download_completed )
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	symbols_collection_type::const_iterator it = d->symbols.find(symbol);
	if (it!=d->symbols.end())
	{
		it->second->add_page(page, download_completed);
	}
}

ptr_stock_symbol stock_symbol_collection::get_symbol( const std::string& symbol )
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	symbols_collection_type::const_iterator it = d->symbols.find(symbol);
	if (it!=d->symbols.end())
	{
		return it->second;
	}
	return ptr_stock_symbol();
}

void stock_symbol_collection::dump()
{
	boost::mutex::scoped_lock lock_it( d->thread_locker ) ;
	std::basic_stringstream<TCHAR> tss;
	tss << _T("Current index is ") << d->m_current_index << _T(", count is ") << d->symbols.size();
	wxLogVerbose(tss.str().c_str());
}