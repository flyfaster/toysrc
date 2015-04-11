#pragma once

struct RealTimeQuotes
{
	time_t nls_time; // NLS Time (ET)
	std::string nls_price; // NLS Price
	int nls_share_volume; // NLS Share Volume
};

typedef boost::shared_ptr<RealTimeQuotes> SharedRealTimeQuotes;

class STOCKCLIENT_API NasdaqParser 
#ifdef USE_LOKI_PIMPL
	: private Loki::PimplOf<NasdaqParser>::Owner
#endif
{
public:
	NasdaqParser();
    ~NasdaqParser(void);
	int GetTotalPageNo();
	size_t GetRecordCount();
	SharedRealTimeQuotes GetRecord(int index);
	int Parse(const std::string& page_content);
	int ParseFile(const std::string& file_name);
	bool IsComplete(std::stringstream& ss);
	int Save(std::stringstream& ss, const std::string& filename);
    int ExtractQuotesData(
        const std::string& page_src, const std::string& filename);
	//************************************
	// Method:    SetQuoteDate
	// Brief:     
	// Returns:   void
	// Parameter: const boost::gregorian::date & quote_date
	//************************************
	static void SetQuoteDate(const boost::gregorian::date& quote_date);
	static time_t GetTime(const boost::gregorian::date& quote_date, const std::string& time_string);
#ifndef USE_LOKI_PIMPL
	struct ImplNasdaqParser* d;
#endif

};


STOCKCLIENT_API std::string middle_string(const std::string& src, 
						  const std::string& prefix, 
						  const std::string& postfix,
						  bool* found = NULL, 
						  size_t* ptr_prefix_pos=NULL, 
						  size_t* post_fix_pos=NULL);
