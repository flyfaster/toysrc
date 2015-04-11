#pragma once

class NasdaqParserTests : public CppUnit::TestCase
{
public:
	CPPUNIT_TEST_SUITE( NasdaqParserTests );
	CPPUNIT_TEST(testGetTotalPageNo);
	CPPUNIT_TEST(testmiddle_string);
	CPPUNIT_TEST(testGetTime);
	CPPUNIT_TEST(testParseSymbol);
    CPPUNIT_TEST(testSave);
	CPPUNIT_TEST_SUITE_END();

public:

public:
	NasdaqParserTests(void);
	~NasdaqParserTests(void);
	void testGetTotalPageNo();
	void testmiddle_string();
	void testGetTime();
	void testParseSymbol();
    void testSave();
};
