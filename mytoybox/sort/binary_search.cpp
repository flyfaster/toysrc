#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE binary_search
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;

size_t find_one(vector<int>& nums)
{
    size_t x = 0;
    for (size_t b = nums.size() - 1; b > 0; b /= 2)
        while (nums[x + b] < 1)
            x += b;
    return x + 1;
}

BOOST_AUTO_TEST_CASE(check_find_one)
{
    cout << "BOOST_AUTO_TEST_CASE(check find_one)" << endl;
    vector<int> nums(7919);
    size_t target = 5711;
    fill(nums.begin(), nums.begin() + target, 0);
    fill(nums.begin() + target, nums.end(), 1);
    BOOST_CHECK_EQUAL(target, find_one(nums));
}

size_t find_summit(vector<int>& nums)
{
    size_t x = 0;
    for (size_t b = nums.size() - 1; b > 0; b /= 2)
        while (nums[x + b] < nums[x + b + 1])
            x += b;
    return x + 1;
}

BOOST_AUTO_TEST_CASE(check_find_summit)
{
    cout << "BOOST_AUTO_TEST_CASE(check find_summit)" << endl;
    vector<int> nums(7919);
    size_t target = 5711;
    iota(nums.begin(), nums.begin() + target, 1);
    generate(nums.begin() + target, nums.end(), [n = 5700]() mutable { return --n; });
    auto res = find_summit(nums);
    BOOST_TEST_CONTEXT("max value(" << nums[target - 1] << ") vs " << nums[res])
    BOOST_CHECK_EQUAL(target - 1, res);
}
