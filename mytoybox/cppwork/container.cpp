#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#include <bitset>
#include <string>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE container
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;

BOOST_AUTO_TEST_CASE(check_reserve)
{
    cout << "BOOST_AUTO_TEST_CASE(check find_one)" << endl;
    vector<int> nums;
    size_t target = 5711;
    nums.push_back(1);
    BOOST_CHECK_EQUAL(1, nums.size());
    nums.reserve(target);
    BOOST_CHECK_EQUAL(1, nums.size());
    nums.push_back(1);
    BOOST_CHECK_EQUAL(2, nums.size());
    nums.clear();
}

BOOST_AUTO_TEST_CASE(check_bitset)
{
    cout << "BOOST_AUTO_TEST_CASE(check bitset)" << endl;
    bitset<10> s(string("0010011010")); // from right to left
    bitset<10> s2 = s;
    BOOST_CHECK_EQUAL(1, s[1]);
    BOOST_CHECK_EQUAL(0, s[s.size()-1]);
    BOOST_CHECK_EQUAL(0, (s^s2).count());
}
