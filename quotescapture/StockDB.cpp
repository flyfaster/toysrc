#include "StdAfx.h"
#include "SqliteSelectCallback.h"
#include "StockDB.h"
#include "sqlite3.h"
#include "ScopeTracer.h"
#include "db_exception.h"
#include <wx/log.h>

struct ImplStockDB
//template<>
//struct Loki::ImplOf<StockDB>
{
	sqlite3* db;
	int CreateTables();
	int Exec(const char* sql_string); 
	int Begin();
	int Commit();
	int Rollback();
	bool in_transaction;
	static const int COMMIT_BATCH_SIZE = 2048;
	boost::mutex mut;

};

//typedef Loki::ImplOf<StockDB> ImplStockDB;
int ImplStockDB::Begin()
{
	if (in_transaction)
		return SQLITE_OK;
	int ret = Exec("BEGIN");
	in_transaction = (SQLITE_OK==ret);
	return ret;
}

int ImplStockDB::Commit()
{
	int ret = SQLITE_OK;
	if (in_transaction)
	{
		in_transaction = false;
		ret = Exec("COMMIT");
	}
	return ret;
}

int ImplStockDB::Rollback()
{
	int ret = SQLITE_OK;
	if (in_transaction)
	{
		in_transaction = false;
		ret = Exec("ROLLBACK");
	}
	return ret;
}

int ImplStockDB::Exec( const char* sql_string )
{
	boost::unique_lock<boost::mutex> lock(mut);

	int ret = SQLITE_OK;
	// throw exception in case of error
	char* errmsg = NULL;
	ret = sqlite3_exec(db, sql_string, NULL, NULL, &errmsg);
	if (ret!=SQLITE_OK)
	{
		throw db_exception(ret, errmsg);
	}
	return ret;	
}

int StockDB::Close()
{
	sqlite3* db = d->db;
	int ret = SQLITE_OK;
	if (db)
	{
		ret = sqlite3_close(db);
		db = NULL;
	}
	d->in_transaction = false;
	return ret;
}

StockDB::StockDB(void)
{
	d = new ImplStockDB;
	d->in_transaction = false;
	d->db = NULL;
}

StockDB::~StockDB(void)
{
	if (d)
		delete d;
	d = NULL;
}

int StockDB::Open()
{
	int ret = SQLITE_OK;
	std::string db_filename = "stockquotes.db";
	boost::filesystem::path db_path(db_filename);
	if (!d->db)
	{
		bool db_exists = boost::filesystem::exists(db_path);
		ret = sqlite3_open(db_filename.c_str(), &d->db);
		if (SQLITE_OK == ret && !db_exists)
		{
			ret = d->CreateTables();
		}
	}
	return ret;
}

int StockDB::AddDailyQuotes( LPCSTR symbol_name, LPCSTR datestr, 
							double open, double high, double low, 
							double close, double volume )
{
	std::stringstream ss;
	ss << "insert into daily values(" 
		<< "'" << symbol_name << "', "
		<< "'" << datestr << "', "
		<< open << ", "
		<< high << ", "
		<< low << ", "
		<< close << ", "
		<< volume <<")";
	int ret = d->Exec(ss.str().c_str());
	return ret;
}

//************************************
// Method:    from_short_dmy
// FullName:  from_short_dmy
// Access:    public 
// Returns:   boost::gregorian::date
// Qualifier:
// Parameter: std::string datestr formats like 17-Mar-08 means 2008-03-17
//************************************
boost::gregorian::date from_short_dmy(std::string datestr) 
{
	using namespace boost::gregorian;
	std::string spec_str("");
	spec_str = "dmy";

	unsigned pos = 0;
	unsigned short year(0), month(0), day(0);
	typedef std::basic_string<char>::traits_type traits_type;
	typedef boost::char_separator<char, traits_type> char_separator_type;
	typedef boost::tokenizer<char_separator_type,
		std::basic_string<char>::const_iterator,
		std::basic_string<char> > tokenizer;
	typedef boost::tokenizer<char_separator_type,
		std::basic_string<char>::const_iterator,
		std::basic_string<char> >::iterator tokenizer_iterator;
	// may need more delimiters, these work for the regression tests
	const char sep_char[] = {'-'};
	char_separator_type sep(sep_char);
	tokenizer tok(datestr,sep);
	for(tokenizer_iterator beg=tok.begin(); 
		beg!=tok.end() && pos < spec_str.size(); 
		++beg, ++pos) {
			switch(spec_str.at(pos)) 
			{
			case 'y': 
				{
					year = boost::lexical_cast<unsigned short>(*beg)+2000;
					break;
				}
			case 'm': 
				{
					month = boost::date_time::month_str_to_ushort<date::month_type>(*beg);
					break;
				}
			case 'd': 
				{
					day = boost::lexical_cast<unsigned short>(*beg);
					break;
				}
			} //switch
	}
	return date(year, month, day);
}

int StockDB::ImportDailyQuotes( LPCSTR symbol_name, LPCSTR csv_file_name )
{
	std::stringstream ss;
	ss << __FUNCTION__ << " load " << symbol_name << ", batch size:" << d->COMMIT_BATCH_SIZE;
	ScopeTracer duration( wxString::FromAscii(ss.str().c_str()));
	std::ifstream daily_quotes_input(csv_file_name);
	std::string one_line;
	std::getline(daily_quotes_input, one_line);
	// skip first line
	int record_count = 0 ;
	try
	{
		while(daily_quotes_input.good() && one_line.length())
		{
			std::getline(daily_quotes_input, one_line);
			if (one_line.length()==0)
				break;
			boost::char_separator<char> sep( (","));
			typedef boost::tokenizer<boost::char_separator<char>,
				std::basic_string<char>::const_iterator,std::basic_string<char> > tokenizer;
			tokenizer tokens(one_line, sep);
			tokenizer::iterator token_iter = tokens.begin();
			std::vector<std::string> tokens_col;
			while (token_iter!=tokens.end())
			{
				tokens_col.push_back(*token_iter);
				token_iter++;
			}
			if (tokens_col.size() == 6)
			{
				// Date,Open,High,Low,Close,Volume
				boost::gregorian::date date(from_short_dmy(tokens_col[0]));
				std::string datestr = boost::gregorian::to_iso_string(date);
				if ( (record_count % d->COMMIT_BATCH_SIZE) ==0)
					d->Begin();
				record_count++;
				if ((record_count%d->COMMIT_BATCH_SIZE)==(d->COMMIT_BATCH_SIZE-1))
					d->Commit();
				AddDailyQuotes(symbol_name, datestr.c_str(), 
					atof(tokens_col[1].c_str()),
					atof(tokens_col[2].c_str()),
					atof(tokens_col[3].c_str()),
					atof(tokens_col[4].c_str()),
					atof(tokens_col[5].c_str()));
			}
		}	
	}
	catch (const db_exception& e)
	{
		wxLogError(wxString::FromAscii(e.what()));
		return e.error();
	}
	d->Commit();
	return SQLITE_OK;
}

shared_SqliteSelectCallback StockDB::Query( LPCSTR sql_string )
{
	shared_SqliteSelectCallback return_records(new SqliteSelectCallback());
	int ret = SQLITE_OK;
	// throw exception in case of error
	char* errmsg = NULL;
	ret = sqlite3_exec(d->db, sql_string, SqliteSelectCallback::Callback, return_records.get(), &errmsg);
	if (ret!=SQLITE_OK)
	{
		throw db_exception(ret, errmsg);
	}
	return return_records;
}

int StockDB::AddQuote( const std::string symbol_name, const std::string datestr, const std::string price_str )
{
	//cout << __FUNCTION__ << "(" << symbol_name << ", " << datestr << ", " << price_str << ")"<< std::endl;
	if (price_str.length()&&datestr.length()&&datestr.find("Close")==std::string::npos)
	{
		std::stringstream ss;
		ss << "INSERT INTO quotes values('" << symbol_name << "', '" << datestr << "', " << price_str << ")";
		return d->Exec(ss.str().c_str());

	}
	return 0;
}

int StockDB::AddRealTimePrice( LPCSTR symbol_name, int volume, float price, time_t transaction_time )
{
	return 0;
}

int ImplStockDB::CreateTables()
{
	int ret = SQLITE_OK;
	std::string quote_stmt = "create table quotes(code integer, pricetime text, price numeric)";
	std::string symbol_code_stmt = "create table symbol_code(symbol text unique, code integer unique, name text, marketcapital numeric, market text)";
	std::string daily_quotes_table_stmt = "create table daily (symbol VARCHAR(10), date char(8), open float, high float, low float, close float, volume, primary key (date, symbol))";
	try
	{
		ret = Exec(quote_stmt.c_str());
		ret = Exec(symbol_code_stmt.c_str());
		ret = Exec(daily_quotes_table_stmt.c_str());
		ret = Exec("create table markets(marketname text)");
		ret = Begin();
		ret = Exec("insert into symbol_code values ('ADSK', 30085, 'Autodesk', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('ADBE', 4112, 'Adobe', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('MTW', 346512, 'MTW', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('TEX', 554272, 'TEX', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('QCOM', 656142, 'QCOM', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('NOK', 657729, 'Nokia', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('NVDA', 662925, 'NVida', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('DNA', 663363, 'Autodesk', 22, 'NASDAQ')");
		ret = Exec("insert into symbol_code values ('VRGY', 705307, 'Verigy', 22, 'NASDAQ')");
		ret = Commit();
		ret = Begin();
		ret = Exec("insert into markets values('NASDAQ')");
		ret = Exec("insert into markets values('NYSE')");
		ret = Commit();

	}
	catch (const db_exception& e)
	{
		ret = e.error();
	}
	return ret;
}