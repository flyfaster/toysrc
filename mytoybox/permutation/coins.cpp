#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#include <numeric>
#include "boost/optional.hpp"
//#ifndef __has_include
//#define __has_include(header) 0
//#endif
//
//#if __has_include(<optional>)
//#include "adaptor/cpp17/optional.hpp"
//#endif

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dp
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;
using boost::optional;

size_t coins(vector<size_t>& faces, size_t target)
{
    vector<optional<size_t>> cache(target + 1, optional<size_t>{});

    cache[0] = 0;

    std::function<size_t(size_t)> search;

    search = [&search, &faces, &cache](size_t t) -> size_t {
        if (cache[t])
            return *cache[t];
        size_t res = numeric_limits<size_t>::max();
        for (auto face : faces)
            if (face <= t)
                res = min(res, search(t - face) + 1);
        cache[t] = res;
        return res;
    };
    return search(target);
}

BOOST_AUTO_TEST_CASE(check_coins)
{
    cout << "BOOST_AUTO_TEST_CASE(check_coins)" << endl;
    vector<size_t> faces{1, 3, 4};
    for (size_t target = 5; target <= 8; ++target)
    {
        BOOST_TEST_CONTEXT("target = " << target)
        BOOST_CHECK_EQUAL(2, coins(faces, target));
    }
    BOOST_CHECK_EQUAL(3, coins(faces, 10));
}
