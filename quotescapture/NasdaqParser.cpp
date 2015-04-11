#include "StdAfx.h"
#include <string>
#include <list>
#include <fstream>
#include <wx/wx.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "NasdaqParser.h"
#include "stock_symbol.h"
#include "stock_symbol_collection.h"
#include "http_client_factory.h"
#include "MainApp.h"
namespace nasdaq_com
{
	const char* TABLE_START_PREFIX = "<table class=\"AfterHoursPagingContents";
	const char* TABLE_END_PREFIX = "</table>";
	const char* CELL_START_PREFIX = "<td>";
	const char* CELL_END_PREFIX = "</td>";
	const char* PRICE_START_PREFIX = "&nbsp;$&nbsp;";
	const char* SYMBOL_PREFIX = "var quoteBoxSelectedSymbol= \"";
	const char* SYMBOL_POSTFIX = "\"";
	const char* TOTAL_PAGE_PREFIX = "<span id=\"TotalPagesLabel\">";
	const char* TOTAL_PAGE_POSTFIX = "</span>";

}

#ifdef USE_LOKI_PIMPL
template <>
struct Loki::ImplOf<NasdaqParser> 
#else
struct ImplNasdaqParser
#endif

{
	int m_total_page_no;
	std::string m_symbol;
	static boost::gregorian::date quote_date;
	int ParseTotalPageNo(const std::string& src);
	//************************************
	// Method:    ParseSymbol
	// Brief:     parse symbol name from the following patter: var quoteBoxSelectedSymbol= "ADSK";
	// Returns:   bool
	// Parameter: const std::string & src
	//************************************
	bool ParseSymbol(const std::string& src); 
	std::vector<boost::shared_ptr<RealTimeQuotes> > m_quotes_table;
};
#ifdef USE_LOKI_PIMPL
typedef Loki::ImplOf<NasdaqParser> ImplNasdaqParser;
#endif
boost::gregorian::date ImplNasdaqParser::quote_date;
std::string middle_string(const std::string& src, 
						  const std::string& prefix, 
						  const std::string& postfix,
						  bool* found /*= NULL*/, 
						  size_t* ptr_prefix_pos/*=NULL*/, 
						  size_t* post_fix_pos/*=NULL*/)
{
	std::string ret;
	std::string::size_type prefix_pos = src.find(prefix);
	std::string::size_type postfix_pos = src.find(postfix, prefix_pos+prefix.length());
	if (ptr_prefix_pos)
		*ptr_prefix_pos = prefix_pos;
	if (post_fix_pos)
		*post_fix_pos = postfix_pos;
	if (prefix_pos!=std::string::npos && postfix_pos !=std::string::npos)
	{
		size_t count = postfix_pos - prefix_pos - prefix.length();
		if (count>0)
		{
			ret = src.substr(prefix_pos+prefix.length(), count);

		}
		if (found)
			*found=true;
	}
	else
	{
		if (found)
			*found = false;
	}
	return ret;
}

int ImplNasdaqParser::ParseTotalPageNo( const std::string& src )
{
	std::string total_page_str = middle_string(src, 
		nasdaq_com::TOTAL_PAGE_PREFIX, nasdaq_com::TOTAL_PAGE_POSTFIX);
	if (total_page_str.length()>0)
	{
		m_total_page_no = atoi(total_page_str.c_str());
	}
	else
	{
		wxLogError(wxT("%s error"), wxString::FromAscii(__FUNCTION__));
	}
	return m_total_page_no;
}

bool ImplNasdaqParser::ParseSymbol( const std::string& src )
{
	m_symbol = middle_string(src, nasdaq_com::SYMBOL_PREFIX, nasdaq_com::SYMBOL_POSTFIX);
	return m_symbol.length()>0;
}

NasdaqParser::NasdaqParser(void)
{
#ifndef USE_LOKI_PIMPL
	d = new ImplNasdaqParser;
#endif

	d->m_total_page_no = 0;
}

NasdaqParser::~NasdaqParser(void)
{
#ifndef USE_LOKI_PIMPL
	delete d;
	d=NULL;
#endif
}

int NasdaqParser::GetTotalPageNo()
{
	return d->m_total_page_no;
}

int NasdaqParser::Parse( const std::string& page_content )
{
	if (d->ParseTotalPageNo(page_content) <=0)
	{
		return 0-__LINE__; // invalid page content
	}
	// parse real time data table
	// the table is between AfterHoursPagingContents_Table and </table>
	std::string::size_type /*table_start_pos=0, table_end_pos=0,*/ td_start=0, td_end=0;
	std::string table_content = middle_string(page_content, 
		nasdaq_com::TABLE_START_PREFIX, nasdaq_com::TABLE_END_PREFIX);
	if (table_content.length()<1)
	{
		wxLogError(wxT("%s data table not found. page content length: %d."), 
			wxString::FromAscii(__FUNCTION__),
			page_content.length());
		return 0-__LINE__;
	}
	// parse values between <td> and </td>.
	std::string::size_type current_position = 0;
	std::list<std::string> data_collection;
	size_t td_char_count = 0;
	while (true)
	{
		td_start = table_content.find(nasdaq_com::CELL_START_PREFIX, current_position);
		td_end = table_content.find(nasdaq_com::CELL_END_PREFIX, td_start);
		current_position = td_end;
		td_char_count = td_end-td_start -strlen(nasdaq_com::CELL_START_PREFIX);
		if (td_char_count>0 && td_start!=std::string::npos && td_end != std::string::npos)
		{
			std::string cell_value = table_content.substr(td_start+strlen(nasdaq_com::CELL_START_PREFIX), td_char_count);
			data_collection.push_back(cell_value);
		}
		else
		{
			break;
		}
	}
	if (data_collection.size() % 3 )
	{
		wxLogError(wxT("%s unexpected cell number: %d"), wxString::FromAscii(__FUNCTION__), data_collection.size());
		return 0-__LINE__;
	}

	d->m_quotes_table.clear();
	d->m_quotes_table.resize( data_collection.size()/3);
	int current_record_no = 0;

	std::locale system_locale("");
#if defined _MSC_VER
	std::locale usa_locale("American_USA.1252");
#else
	std::locale usa_locale("en_US.UTF-8");
#endif
	std::string price_padding = "&nbsp;$&nbsp;";
	std::locale::global( usa_locale );
	while (data_collection.size())
	{
		// {16:00:00 , &nbsp;$&nbsp;1.57 &nbsp;, 1,042
		boost::shared_ptr<RealTimeQuotes> ptr_quote(new RealTimeQuotes);
		std::string time_str = data_collection.front();
		data_collection.pop_front();
		std::string price_str = data_collection.front();
		 price_str = price_str.substr(price_padding.length());
		 boost::replace_all(price_str, "&nbsp;", "");
		 //boost::replace_all(price_str, " ", "");
		 boost::algorithm::trim(price_str);
		data_collection.pop_front();
		std::string volume_str = data_collection.front();
		data_collection.pop_front();
		//strptime();
		ptr_quote->nls_time = GetTime(d->quote_date, time_str);
		if ( ptr_quote->nls_time == 0)
		{
			wxLogError(wxT("%s error input %s"), wxString::FromAscii(__FUNCTION__), 
				wxString::FromAscii(time_str.c_str()));
			break; // invalid format
		}

		std::istringstream  iStrStrm( volume_str );
		ptr_quote->nls_share_volume = 0;
		iStrStrm >> ptr_quote->nls_share_volume;
		float price = 0;
		sscanf_s(price_str.c_str(), "%f", &price);
		ptr_quote->nls_price = price_str;
		d->m_quotes_table[current_record_no] = ptr_quote;
		current_record_no++;
	}
	std::locale::global( system_locale );
	return 0;
}

size_t NasdaqParser::GetRecordCount()
{
	return d->m_quotes_table.size();
}

bool NasdaqParser::IsComplete( std::stringstream& ss )
{
	const size_t rough_length = 60*1024;
	//if (ss.rdbuf()->si)
	return false;
}

int NasdaqParser::Save( std::stringstream& ss, const std::string& filename )
{
	int ret = 0;
	std::string page_content = ss.str();

	size_t start = page_content.find(nasdaq_com::TABLE_START_PREFIX);
	size_t end = page_content.find(nasdaq_com::TABLE_END_PREFIX, start);
	std::string save_file = filename;
	if (start ==std::string::npos || end==std::string::npos)
	{
		save_file += ".bad";
		ret = 0 - __LINE__;
	}
	if (d->ParseTotalPageNo(page_content) >0)
	{
		// get stock symbol
		d->ParseSymbol(page_content);
	}
	if (d->m_symbol.length()==0)
    {
        wxLogVerbose( wxT("%s %s invalid symbol"), 
            wxString::FromAscii(__FUNCTION__), 
            wxString::FromAscii(save_file.c_str()));
        return 0-__LINE__;
    }
    if (d->m_total_page_no<=0)
    {
        wxLogVerbose( wxT("%s %s invalid total page number"), 
            wxString::FromAscii(__FUNCTION__), 
            wxString::FromAscii(save_file.c_str()));
        return 0-__LINE__;
    }
	std::ofstream output_file;
	output_file.open(save_file.c_str());
	if (output_file.good())
	{
		output_file << ss.str();
		output_file.close();
        wxLogVerbose( wxT("%s %s download complete"), 
            wxString::FromAscii(__FUNCTION__), 
            wxString::FromAscii(save_file.c_str()));
		//return ret;
	}
	else
	{
		wxLogError(wxT("%s open file %s error"), wxString::FromAscii(__FUNCTION__),
			wxString::FromAscii(save_file.c_str()));
		return 0-__LINE__;
	}
    // save quotes table to VRGY 20090529page001.<total page num>.txt
    // VRGY 20090529page001.txt
    std::string::size_type dot_position = filename.find_last_of('.');
    if (dot_position!=std::string::npos)
    {
        std::string table_data_filename = filename.substr(0, dot_position);
        char buf[32];
        sprintf_s(buf, "-%03d.data", d->m_total_page_no);
        table_data_filename += buf;
        ret = ExtractQuotesData(ss.str(), table_data_filename);
        if (ret)
            return ret;
    }
    // add download tasks
    ptr_stock_symbol pstock = SingleSymbolCol::Instance().get_symbol(d->m_symbol);
    if (!pstock)
    {
        SingleSymbolCol::Instance().add_symbol(d->m_symbol, d->m_total_page_no);
        wxGetApp().GetHttpClientFactory()->add_total_task(d->m_symbol, d->m_total_page_no);
    }
    if (pstock && pstock->total_pages()<1)
    {
        // 
        wxGetApp().GetHttpClientFactory()->add_total_task(d->m_symbol, d->m_total_page_no);
    }

	return ret;
}

time_t NasdaqParser::GetTime( const boost::gregorian::date& quote_date, const std::string& time_str )
{
	int hour, minute, second;
	int scanret = sscanf(time_str.c_str(), "%d:%d:%d", &hour, &minute, &second);
	if (scanret!=3)
	{
		return 0;
	}
	using boost::posix_time::hours;
	using boost::posix_time::minutes;
	using boost::posix_time::seconds;
	boost::posix_time::ptime ny_time(ImplNasdaqParser::quote_date,
		hours(hour)+minutes(minute)+seconds(second) ); // this is NY time
	// need to convert ny to local
	boost::posix_time::ptime local_time = MainApp::NYToLocal(ny_time);
	tm pt_tm = boost::posix_time::to_tm(local_time);
	time_t ret_time = mktime(&pt_tm);
	return ret_time;
}

void NasdaqParser::SetQuoteDate(const boost::gregorian::date& quote_date)
{
	ImplNasdaqParser::quote_date = quote_date;
}

int NasdaqParser::ParseFile( const std::string& file_name )
{
	using boost::filesystem::path;
	path pfile(file_name, boost::filesystem::native);
	size_t file_size = boost::filesystem::file_size(pfile);
	if (file_size <=0)
	{
		return 0-__LINE__; // invalid page content
	}
	std::string fn = pfile.filename(); // F 20090222T192335.htm
	size_t space_pos = fn.find(' ');
	if (space_pos >0)
	{
		int ndate = 0;
		fn = fn.substr(space_pos+1);
		int ret = sscanf(fn.c_str(), "%d", &ndate);
		if (ret && ndate>0)
		{
			int ny = ndate /10000;
			int nm = (ndate % 10000)/100;
			int nd = (ndate % 100);
			if (ny<1970 || ny>2050)
			{
				return 0-__LINE__;
			}
			if (nm<=0 || nm>12)
			{
				return 0-__LINE__;
			}
			if (nd<=0 || nd>31)
			{
				return 0-__LINE__;
			}
			boost::gregorian::date dummy_date(ny, nm, nd);
			NasdaqParser::SetQuoteDate(dummy_date);

		}
	}
	else
	{
		return 0-__LINE__; // invalid page content
	}

	std::ifstream infile;
	infile.open( wxString::FromAscii(file_name.c_str()));
	boost::scoped_array<char> buf(new char[file_size+1]);
	if (!buf.get())
	{
		return 0-__LINE__; // invalid page content
	}
	infile.read(buf.get(), file_size);
	buf[file_size] = 0;
	std::string page_content = buf.get();
	return Parse(page_content);
}

SharedRealTimeQuotes NasdaqParser::GetRecord( int index )
{
	if (index >=0 && index < (int)d->m_quotes_table.size())
	{
		return d->m_quotes_table[index];
	}
	return SharedRealTimeQuotes();
}

int NasdaqParser::ExtractQuotesData(
                      const std::string& page_src, 
                      const std::string& filename)
{
    std::string::size_type table_start_position, table_end_position;
    table_start_position = page_src.find(nasdaq_com::TABLE_START_PREFIX);
    table_end_position = page_src.find(nasdaq_com::TABLE_END_PREFIX, table_start_position);
    if (table_start_position==std::string::npos
        || table_end_position == std::string::npos)
    {
        return 0-__LINE__;
    }
    std::string table_content = page_src.substr(table_start_position, 
        table_end_position+strlen(nasdaq_com::TABLE_END_PREFIX)-table_start_position);
    std::ofstream fsave;
    fsave.open(filename.c_str());
    if (fsave.good())
        fsave<<table_content;
    if (fsave.good())
        return 0;
    return 0-__LINE__;
}
