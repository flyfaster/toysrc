#include <vector>
#include <iostream>
#include <random>
#include <numeric>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE sparse_table
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE(check_sparse_table)
{
	constexpr int n = 100;
	const int k = (int)std::log2(n);
    vector<int> Arr(n);
    vector<vector<int>> table(n, vector<int>(k+1));

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, n);
    for (int i = 0; i < n; i++)
        Arr[i] = dist(rd);

    vector<int> nums = Arr;

    // build Sparse Table
    for (int i = 0; i < n; i++)
        table[i][0] = Arr[i];

    for (int j = 1; (1 << j) <= n; ++j)
    {
        for (int i = 0; i <= n - (1 << j); ++i)
            table[i][j] = table[i][j - 1] + table[i + (1 << (j - 1))][j - 1];
    }

    auto range_sum_from_sparse_table = [&table](int L, int R) {
        long long answer = 0;
        int k = (R <= L) ? 0 : ((int) std::ceil(std::log2(R - L)) + 1);

        for (int j = k; j >= 0; j--)
        {
            if (L + (1 << j) - 1 <= R)
            {
                answer = answer + table[L][j];
                L += 1 << j;
            }
        }
        return answer;
    };

    std::partial_sum(nums.begin(), nums.end(), nums.begin());
    auto get_sum = [&nums](int L, int R) {
        int left = (L > 0) ? nums[L - 1] : 0;
        return nums[R] - left;
    };

    for (int L = 0; L < n; ++L)
        for (int R = L; R < n; ++R)
        {
            BOOST_TEST_CONTEXT("range sum(" << L << "," << R << ")")
            BOOST_CHECK_EQUAL(get_sum(L, R), range_sum_from_sparse_table(L, R));
        }

    cout << "log2(3.9)=" << std::log2(3.9) << "\n";
    BOOST_CHECK_EQUAL(1, (int) std::log2(3.9));
    BOOST_CHECK_EQUAL(2, (int) std::lrint(std::log2(3.9)));
}
