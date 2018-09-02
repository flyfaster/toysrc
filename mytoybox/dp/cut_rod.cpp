#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cut_rod
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

int cut_rod_recursive(vector<int> const& price, int rod_length)
{
    if (rod_length == 0)
        return 0;
    int res = numeric_limits<int>::min();
    for (int i = 1; i < price.size() && i <= rod_length; i++)
        res = max(res, price[i] + cut_rod_recursive(price, rod_length - i));
    return res;
}

int cut_rod_dp_top_down(vector<int> const& price, int rod_length)
{
    if (rod_length == 0)
        return 0;
    vector<int> dp(rod_length + 1, numeric_limits<int>::min());
    dp[0] = 0;

    std::function<int(int)> dp_top_down;
    dp_top_down = [&dp_top_down, &dp, &price](int rl) -> int {
    	int& res = dp[rl];

        if (res > numeric_limits<int>::min())
            return res;

        for (int i = 1; i < price.size() && i <= rl; ++i)
            res = max(res, price[i] + dp_top_down(rl - i));
        return res;
    };

    return dp_top_down(rod_length);
}

int cut_rod_dp_bottom_up(vector<int> const& price, int rod_length)
{
    vector<int> dp(rod_length + 1, numeric_limits<int>::min());
    dp[0] = 0;
    for (int i = 1; i <= rod_length; ++i)
    {
        for (int j = 1; j < price.size() && j <= i; ++j)
            dp[i] = max(dp[i], dp[i - j] + price[j]);
    }
    return dp[rod_length];
}

BOOST_AUTO_TEST_CASE(check_cut_rod_recursive)
{
    cout << "BOOST_AUTO_TEST_CASE(cut_rod_recursive)" << endl;
    vector<int> prices{0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30};
    int len = 4; int max_value = 10;
    BOOST_TEST_CONTEXT("cut_rod_recursive(" << len << ")")
    BOOST_CHECK_EQUAL(max_value, cut_rod_recursive(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_top_down(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_bottom_up(prices, len));
    len = 7; max_value = 18;
    BOOST_TEST_CONTEXT("cut_rod_recursive(" << len << ")")
    BOOST_CHECK_EQUAL(max_value, cut_rod_recursive(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_top_down(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_bottom_up(prices, len));
    len = 0; max_value = 0;
    BOOST_TEST_CONTEXT("cut_rod_recursive(" << len << ")")
    BOOST_CHECK_EQUAL(max_value, cut_rod_recursive(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_top_down(prices, len));
    BOOST_CHECK_EQUAL(max_value, cut_rod_dp_bottom_up(prices, len));
}
