#include  <afx.h>
#include "stdafx.h"
#include "StockClientUT.h"
#include "NasdaqParserTests.h"
#include "../NasdaqParser.h"
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NasdaqParserTests, STOCK_CLIENT_UT_SUITE );

NasdaqParserTests::NasdaqParserTests(void)
{
}

NasdaqParserTests::~NasdaqParserTests(void)
{
}

void NasdaqParserTests::testGetTotalPageNo()
{
	boost::gregorian::date dummy_date(2000, boost::date_time::Jan, 1);
	NasdaqParser::SetQuoteDate(dummy_date);
	std::ifstream input_file;
	input_file.open("E:\\svn\\stockclient\\Debug\\VRGY 20090222T192335.txt");
	std::string one_line;
	std::stringstream ss;
	while(input_file.good())
	{
		one_line.clear();
		getline(input_file, one_line);
		//input_file >> one_line;
		ss << one_line;
	}
	one_line = ss.str();
	wxLogVerbose(wxT("read %d bytes"), one_line.size());
NasdaqParser test_parser;
test_parser.Parse(one_line);
int page_count = test_parser.GetTotalPageNo();
CPPUNIT_ASSERT_EQUAL_MESSAGE( ("total page number"),109 , page_count);

}

void NasdaqParserTests::testmiddle_string()
{
	std::string src = "var quoteBoxSelectedSymbol= \"ADSK\";";
	std::string symbol = middle_string(src, "var quoteBoxSelectedSymbol= \"",
		"\"");
	CPPUNIT_ASSERT_EQUAL(std::string("ADSK"), symbol);
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	//Sleep(1000);
	boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time()-now;
	char buf[128];
	sprintf_s(buf, ("Diff is %d."), diff.total_milliseconds());
	//CPPUNIT_ASSERT_EQUAL_MESSAGE( buf, 1000, (int)diff.total_milliseconds());
}

void NasdaqParserTests::testGetTime()
{
	boost::gregorian::date dummy_date(2000, boost::date_time::Jan, 1);
	NasdaqParser::SetQuoteDate(dummy_date);

	std::string time_string = "16:00:05 ";
	time_t ret_time = NasdaqParser::GetTime(dummy_date, time_string);
	boost::posix_time::ptime test_time = boost::posix_time::from_time_t(ret_time);
	CPPUNIT_ASSERT_EQUAL( dummy_date, test_time.date());
	boost::posix_time::ptime::time_duration_type time_components = test_time.time_of_day();
	CPPUNIT_ASSERT_EQUAL(0, (int)time_components.minutes());
	CPPUNIT_ASSERT_EQUAL(5, (int)time_components.seconds());
	CPPUNIT_ASSERT_EQUAL(16, (int)time_components.hours());
}

void NasdaqParserTests::testParseSymbol()
{
	//boost::gregorian::date dummy_date(2009, boost::date_time::Feb, 22);
	//NasdaqParser::SetQuoteDate(dummy_date);

	NasdaqParser parser;
	int ret = parser.ParseFile("E:\\svn\\stockclient\\Nasdaq.com\\F 20090222T192335.htm");
	CPPUNIT_ASSERT_EQUAL(0, ret);
	CPPUNIT_ASSERT_EQUAL(470, parser.GetTotalPageNo());
	CPPUNIT_ASSERT_EQUAL(50, (int)parser.GetRecordCount());
	SharedRealTimeQuotes rec = parser.GetRecord(0);
	CPPUNIT_ASSERT_EQUAL(200, rec->nls_share_volume);
	CPPUNIT_ASSERT_EQUAL( std::string("1.58"), rec->nls_price);
	rec = parser.GetRecord(1);
	CPPUNIT_ASSERT_EQUAL(283, rec->nls_share_volume);
	rec = parser.GetRecord(49);
	CPPUNIT_ASSERT_EQUAL(666, rec->nls_share_volume);
	CPPUNIT_ASSERT_EQUAL( std::string("1.58"), rec->nls_price);

}

void NasdaqParserTests::testSave()
{
    std::stringstream ss;
    std::ifstream fsrc;
    fsrc.open("D:\\svn\\quotescapture\\data\\nlstrades.aspx");
    CPPUNIT_ASSERT(fsrc.good());
    while(fsrc.good() && fsrc.eof()==false)
    {
        std::string oneline;
        //fsrc.getline(oneline);
        getline(fsrc, oneline);
        ss<<oneline<<std::endl;
    }
    NasdaqParser nasdaq_parser;
    int ret = nasdaq_parser.Save(ss,  "testSave.htm");
    CPPUNIT_ASSERT_EQUAL(0, ret);
}