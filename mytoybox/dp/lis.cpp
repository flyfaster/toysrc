#include <cstdio>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <vector>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE lis
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

int max_sum_inc_seq(const std::vector<int>& arr)
{
    std::vector<int> msis = arr;

    /* Compute maximum sum values in bottom up manner */
    for (int i = 1; i < arr.size(); i++)
        for (int j = 0; j < i; j++)
            if (arr[i] > arr[j] && msis[i] < msis[j] + arr[i])
                msis[i] = msis[j] + arr[i];

    return *std::max_element(msis.begin(), msis.end());
}

BOOST_AUTO_TEST_CASE(check_longest_increasing_subsequence_max_sum)
{
    std::vector<int> arr{1, 101, 2, 4, 100, 4, 5};
    BOOST_TEST_CONTEXT("longest sequence is 1 2 4 100")
    BOOST_CHECK_EQUAL(107, max_sum_inc_seq(arr));
}
