#include "stdafx.h"
#include <map>

#include "stock_symbol.h"

template <> struct Loki::ImplOf<stock_symbol>
{
	std::string name;
	std::string symbol;
	int code;
	int total_page_no;
	std::map<int, int> page_map; // which page is downloaded, which page is in progress, which is not downloaded
	void init();
	enum {NOT_STARTED, IN_PROGRESS, DOWNLOAD_DONE, DOWNLOAD_BAD};
};
typedef Loki::ImplOf<stock_symbol> impl_stock_symbol;

void impl_stock_symbol::init()
{
	code = -1;
	total_page_no = 0;
}

stock_symbol::stock_symbol(void)
{
}

stock_symbol::~stock_symbol(void)
{
}

const std::string& stock_symbol::name()
{
return d->name;
}

void stock_symbol::name( const std::string& n )
{
d->name = n;
}
const std::string& stock_symbol::symbol()
{
return d->symbol;
}

void stock_symbol::symbol( const std::string& s )
{
d->symbol = s;
}
int stock_symbol::code()
{
return d->code;
}

void stock_symbol::code( int c )
{
d->code = c;
}
int stock_symbol::total_pages()
{
return d->total_page_no;
}

void stock_symbol::total_pages( int t )
{
d->total_page_no=t;
}

void stock_symbol::add_page( int page, bool download_completed )
{
	if (download_completed)
		d->page_map[page] = impl_stock_symbol::DOWNLOAD_DONE;
	else
		d->page_map[page] = impl_stock_symbol::NOT_STARTED;
}

int stock_symbol::get_next_page()
{
	for(std::map<int, int>::iterator it=d->page_map.begin();
		it!=d->page_map.end();
		it++)
	{
		if (it->second == impl_stock_symbol::NOT_STARTED)
		{
			it->second = impl_stock_symbol::IN_PROGRESS;
			return it->second;
		}
	}
	return -1;
}