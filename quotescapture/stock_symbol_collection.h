#pragma once
//#include <loki/Pimpl.h>
//#include <string>
class stock_symbol;
typedef boost::shared_ptr<stock_symbol> ptr_stock_symbol;

class STOCKCLIENT_API stock_symbol_collection 
#ifdef USE_LOKI_PIMPL
	: private Loki::PimplOf<stock_symbol_collection>::Owner
#endif
{
public:
	stock_symbol_collection(void);
	~stock_symbol_collection(void);
	// load list from text file, one stock symbol per line
	int load(const std::string& filename);
	std::string pop_symbol();
	void push(const std::string& symbol);
	size_t size();
	bool is_end();
	void add(const std::string& symbol, const std::string& quote_content);

	static void GetMidString( const std::string &quote_content, const std::string &price_token_prefix, 
		const std::string& price_token_postfix, std::string &price );
	void add_symbol(const std::string& symbol);
	void add_symbol(const std::string& symbol, int toal_page_no);
	void add_symbol_page(const std::string& symbol, int page, bool download_completed);
	ptr_stock_symbol get_symbol(const std::string& symbol);
	void dump();
	static stock_symbol_collection& Instance();
#ifndef USE_LOKI_PIMPL
	struct impl_stock_symbol_collection* d;
#endif

};
typedef stock_symbol_collection SingleSymbolCol;
