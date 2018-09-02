#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE edit_distance
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

struct myhash
{
    size_t operator()(pair<int, int> const& hs) const
    {
        return hs.first * 512 + hs.second;
    }
};

class edit_distance
{
public:
    using pi = pair<int, int>;
    unordered_map<pi, int, myhash> memo;
    vector<vector<int>> dp;

    int ed_hash(string const& lhs, int lp, string const& rhs, int rp)
    {
        auto iter = memo.find(pi{lp, rp});
        if (iter != memo.end())
            return iter->second;
        int res = numeric_limits<int>::max();
        if (lhs[lp - 1] == rhs[rp - 1])
            res = ed_hash(lhs, lp - 1, rhs, rp - 1);
        else
        {
            res =
                min(res, 1 + ed_hash(lhs, lp - 1, rhs, rp)); // delete one char from first word
            res = min(res, 1 + ed_hash(lhs, lp, rhs, rp - 1)); // delete one char from 2nd word
            res = min(res, 1 + ed_hash(lhs, lp - 1, rhs,
                                  rp - 1)); // replace one char to make a match
        }
        memo[pi{lp, rp}] = res;
        return res;
    }

    int ed_vec2(string const& lhs, int lp, string const& rhs, int rp)
    {
        if (dp[lp][rp] < numeric_limits<int>::max())
            return dp[lp][rp];

        int& res = dp[lp][rp];

        if (lhs[lp - 1] == rhs[rp - 1])
            res = ed_vec2(lhs, lp - 1, rhs, rp - 1);
        else
        {
            res = min(res,
                      1 + ed_vec2(lhs, lp - 1, rhs, rp)); // delete one char from first word
            res =
                min(res, 1 + ed_vec2(lhs, lp, rhs, rp - 1)); // delete one char from 2nd word
            res = min(res, 1 + ed_vec2(lhs, lp - 1, rhs,
                                   rp - 1)); // replace one char to make a match
        }

        return res;
    }

    int minDistance(string word1, string word2)
    {
        for (int i = 0; i <= word1.size(); ++i)
        {
            vector<int> d(word2.size()+1, numeric_limits<int>::max());
            d[0] = i;
            dp.emplace_back(move(d));
        }

        iota(dp[0].begin(), dp[0].end(), 0);
        return ed_vec2(word1, word1.size(), word2, word2.size());
    }

    int minDistance_hash(string word1, string word2)
    {
		for (int i = 0; i <= word2.size(); ++i)
			memo[{0, i}] = i;
        for (int i = 0; i <= word1.size(); ++i)
            memo[{i, 0}] = i;
        return ed_hash(word1, word1.size(), word2, word2.size());
    }

    int minDistance_vec1(string word1, string word2)
    {
        int len1 = word1.length();
        int len2 = word2.length();
        vector<int> cur(len1 + 1, 0);
        iota(cur.begin(), cur.end(), 0);

        for (int p2 = 1; p2 <= len2; p2++) // each char in word2
        {
            int pre = cur[0];
            cur[0] = p2;
            for (int p1 = 1; p1 <= len1; p1++) // each char in word1
            {
                int temp = cur[p1];
                if (word1[p1 - 1] == word2[p2 - 1])
                    cur[p1] = pre;
                else
                    cur[p1] = min(pre + 1, min(cur[p1] + 1, cur[p1 - 1] + 1));
                pre = temp;
            }
        }
        return cur[len1];
    }
};

BOOST_AUTO_TEST_CASE(check_edit_distance)
{
    cout << "BOOST_AUTO_TEST_CASE(edit distance)" << endl;
    string w1 = "horse";
    string w2 = "ros";
    BOOST_TEST_CONTEXT("check edit distance(" << w1 << "," << w2 << ")")
    BOOST_CHECK_EQUAL(3, edit_distance{}.minDistance(w1, w2));
    BOOST_CHECK_EQUAL(3, edit_distance{}.minDistance_vec1(w1, w2));
    w2 = "hello";
    BOOST_TEST_CONTEXT("check edit distance(" << w1 << "," << w2 << ")")
    BOOST_CHECK_EQUAL(edit_distance{}.minDistance(w1, w2),
                      edit_distance{}.minDistance_vec1(w1, w2));
}
