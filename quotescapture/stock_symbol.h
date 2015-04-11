#pragma once

class stock_symbol : private Loki::PimplOf<stock_symbol>::Owner
{
public:
	stock_symbol(void);
	~stock_symbol(void);
	const std::string& name();
	const std::string& symbol();
	int code();
	int total_pages();
	void name(const std::string& n);
	void symbol(const std::string& s);
	void code(int c);
	void total_pages(int t);
	void add_page(int page, bool download_completed);
	int get_next_page();
};

typedef boost::shared_ptr<stock_symbol> ptr_stock_symbol;