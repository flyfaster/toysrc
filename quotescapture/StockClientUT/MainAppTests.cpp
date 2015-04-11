#include  <afx.h>
#include "stdafx.h"
#include "StockClientUT.h"
#include "NasdaqParserTests.h"
#include "MainAppTests.h"
#include "../MainApp.h"
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MainAppTests, STOCK_CLIENT_UT_SUITE );

MainAppTests::MainAppTests(void)
{
}

MainAppTests::~MainAppTests(void)
{
}

void MainAppTests::testLocalToNY()
{
	// sh time Feb 26, 2009 14:20, ny time: Feb 26, 2009 1:20 AM
	{
		boost::gregorian::date thedate(2009, boost::date_time::Feb, 26);
		boost::posix_time::ptime sh_time(thedate, boost::posix_time::hours(14));  
		boost::posix_time::ptime ny_time = MainApp::LocalToNY(sh_time);
		std::stringstream ss;
		ss << "local time " << sh_time << ", ny time " << ny_time;
		CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  1, (int)ny_time.time_of_day().hours());
	}
	// sh time Mar 26, 2009 14:20, ny time: Mar 26, 2009 2:20 AM
	{
		boost::gregorian::date thedate(2009, boost::date_time::Mar, 26);
		boost::posix_time::ptime sh_time(thedate, boost::posix_time::hours(14));  
		boost::posix_time::ptime ny_time = MainApp::LocalToNY(sh_time);
		std::stringstream ss;
		ss << "local time " << sh_time << ", ny time " << ny_time;
		CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  2, (int)ny_time.time_of_day().hours());
	}
	// margin test 
	// sh time Mar 26, 2009 14:20, ny time: Mar 26, 2009 2:20 AM
	{
		boost::gregorian::date thedate(2009, boost::date_time::Mar, 8);
		boost::posix_time::ptime sh_time(thedate, boost::posix_time::hours(15));  
		boost::posix_time::ptime ny_time = MainApp::LocalToNY(sh_time);
		std::stringstream ss;
		ss << "local time " << sh_time << ", ny time " << ny_time;
		CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  3, (int)ny_time.time_of_day().hours());
	}

}

void MainAppTests::testLocalToNYMarketDate()
{
	using boost::gregorian::date;
	using boost::gregorian::days;
	using boost::posix_time::ptime;
	using boost::posix_time::hours;
	using boost::posix_time::minutes;
	{
		date thedate(2009, boost::date_time::Feb, 26);
		ptime ny_time(thedate, hours(9)+minutes(30));  
		ptime ny_market_time = MainApp::NYToMarketDate(ny_time);
		std::stringstream ss;
		ss << "current NY time " << ny_time << ", market date " << ny_market_time;
		CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  thedate, ny_market_time.date());
	}
	{
		date thedate(2009, boost::date_time::Feb, 26);
		ptime ny_time(thedate, hours(9)+minutes(29));  
		ptime ny_market_time = MainApp::NYToMarketDate(ny_time);
		std::stringstream ss;
		ss << "current NY time " << ny_time << ", market date " << ny_market_time;
		CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  thedate-days(1), ny_market_time.date());
	}
	{
		date thedate(2009, boost::date_time::Feb, 20);
		{
			ptime ny_time(thedate+days(1), hours(9)+minutes(29));  
			ptime ny_market_time = MainApp::NYToMarketDate(ny_time);
			std::stringstream ss;
			ss << "current NY time " << ny_time << ", market date " << ny_market_time;
			CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  thedate, ny_market_time.date());
		}
		{
			ptime ny_time(thedate+days(2), hours(9)+minutes(29));  
			ptime ny_market_time = MainApp::NYToMarketDate(ny_time);
			std::stringstream ss;
			ss << "current NY time " << ny_time << ", market date " << ny_market_time;
			CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  thedate, ny_market_time.date());
		}
		{
			ptime ny_time(thedate+days(3), hours(9)+minutes(29));  
			ptime ny_market_time = MainApp::NYToMarketDate(ny_time);
			std::stringstream ss;
			ss << "current NY time " << ny_time << ", market date " << ny_market_time;
			CPPUNIT_ASSERT_EQUAL_MESSAGE( ss.str(),  thedate, ny_market_time.date());
		}
	}

}