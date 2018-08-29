#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE combinatorics
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

int integer_sum_ways(int n)
{
    if (n == 0)
        return 1;
    int res = 0;
    for (int i = 0; i < n; i++)
        res += integer_sum_ways(i);
    return res;
}

int integer_sum_ways2(int n)
{
    vector<int> cache(n + 1, 0);
    cache[0] = 1;
    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j < i; ++j)
            cache[i] += cache[j];
    }
    return cache[n];
}

BOOST_AUTO_TEST_CASE(check_integer_sum_ways)
{
    cout << "BOOST_AUTO_TEST_CASE(counting the number of ways to represent an integer n "
            "as a sum of positive integers.)"
         << endl;
    int n = 9;

    for (int i = 1; i < n; ++i)
    {
        int expected = 1 << (i - 1);
        auto res = integer_sum_ways(i);
        BOOST_TEST_CONTEXT("integer_sum_ways(" << i << ")")
        BOOST_CHECK_EQUAL(res, expected);
    }

    for (int i = 1; i < n; ++i)
    {
        int expected = 1 << (i - 1);
        BOOST_TEST_CONTEXT("integer_sum_ways2(" << i << ")")
        BOOST_CHECK_EQUAL(integer_sum_ways2(i), expected);
    }
}
