#pragma once
//#include "SqliteSelectCallback.h"
class SqliteSelectCallback;
typedef boost::shared_ptr<SqliteSelectCallback> shared_SqliteSelectCallback;

class STOCKCLIENT_API StockDB //: private Loki::PimplOf<StockDB>::Owner
{
public:
	StockDB(void);
	~StockDB(void);
	int Open();
	//************************************
	// Method:    Close
	// FullName:  StockDB::Close
	// Access:    public 
	// Returns:   int = 0 if successful, otherwise return sqlite3 error code.
	// Qualifier:
	//************************************
	int Close();
	int AddDailyQuotes(LPCSTR symbol_name, LPCSTR datestr, 
		double open, double high, double low, 
		double close, double volume);

	int ImportDailyQuotes(LPCSTR symbol_name, LPCSTR csv_file_name);

	shared_SqliteSelectCallback Query(LPCSTR sql_string);

	int AddQuote(const std::string symbol_name, const std::string datestr, const std::string price_str);

	int AddRealTimePrice(LPCSTR symbol_name, int volume, float price, time_t transaction_time);

	struct ImplStockDB* d;
	
};

typedef Loki::SingletonHolder<StockDB, Loki::CreateUsingNew, Loki::PhoenixSingleton> SingleStockDB;
