#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE complexity
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

size_t linear_func(size_t n, size_t& cnt)
{
    ++cnt;
    if (n <= 1)
        return 1;
    return 1 + linear_func(n - 1, cnt);
}

size_t exp_func(size_t n, size_t& cnt)
{
    // there are O(n) algorithm and O(log n) algorithm
    // ICS 161: Design and Analysis of Algorithms
    // Lecture notes for January 9, 1996
    // https://www.ics.uci.edu/~eppstein/161/960109.html
    ++cnt;
    if (n <= 1)
        return n;
    return exp_func(n - 1, cnt) + exp_func(n - 2, cnt);
}

BOOST_AUTO_TEST_CASE(check_complexity)
{
    cout << "BOOST_AUTO_TEST_CASE(check_complexity)" << endl;
    size_t cnt = 0;

    size_t n = 10000;
    linear_func(n, cnt);
    BOOST_TEST_CONTEXT("linear_func(" << n << ") counter is " << cnt)
    BOOST_CHECK_EQUAL(cnt, n);

    auto golden_ratio = (1.0 + sqrt(5)) / 2.0;
    for (n = 4; n < 10; ++n)
    {
        cnt = 0;
        exp_func(n, cnt);
        BOOST_TEST_CONTEXT("exp_func(" << n << ") counter is " << cnt)
        BOOST_CHECK(cnt > (size_t) std::pow(golden_ratio, n));
        BOOST_CHECK(cnt < (size_t) std::pow(golden_ratio, n + 1));
    }
}
