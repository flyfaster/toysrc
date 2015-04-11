#pragma once

class MainAppTests :
	public CppUnit::TestCase
{
	CPPUNIT_TEST_SUITE( MainAppTests );
	CPPUNIT_TEST(testLocalToNY);
	CPPUNIT_TEST(testLocalToNYMarketDate);
	CPPUNIT_TEST_SUITE_END();
public:
	MainAppTests(void);
	~MainAppTests(void);
	void testLocalToNY();
	void testLocalToNYMarketDate();
};
