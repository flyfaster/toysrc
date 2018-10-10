#include <vector>
#include <iostream>
#include <string>

using namespace std;
// simpler version regex match

// pattern has * and .
bool isMatch(string s, string pattern)
{
    int m = s.length(), n = pattern.length();
    vector<vector<bool>> dp(m + 1, vector<bool>(n + 1, false));
    dp[0][0] = true;

    for (int i = 0; i <= m; i++)
        for (int j = 1; j <= n; j++)
            if (pattern[j - 1] == '*')
                dp[i][j] =
                    dp[i][j - 2] ||
                    (i > 0 && (s[i - 1] == pattern[j - 2] || pattern[j - 2] == '.') && dp[i - 1][j]);
            else
                dp[i][j] = i > 0 && dp[i - 1][j - 1] &&
                           (s[i - 1] == pattern[j - 1] || pattern[j - 1] == '.');
    return dp[m][n];
}

using iter = string::iterator;

// pattern has * and +
bool reg(iter is, iter ie, iter rs, iter re)
{
    if (is == ie && rs == re)
        return true;

    if (rs == re)
        return false;

    if (*rs == '+')
    {
        if (reg(is, ie, rs + 1, re))
            return true; // advance pattern but not input (empty character)

        return (is != ie) &&
               reg(is + 1, ie, rs + 1,
                   re); // advance input and pattern, if input not at the end
    }

    if (*rs == '*')
    {
        if (reg(is, ie, rs + 1, re))
            return true; // advance pattern but not input (empty sequence)

        return (is != ie) &&
               (reg(is + 1, ie, rs, re) ||
                reg(is + 1, ie, rs + 1,
                    re)); // advance input but not patter OR advance input and pattern
    }
    return (is != ie) && (*is == *rs) &&
           reg(is + 1, ie, rs + 1, re); // if they match recurse
}

static bool isMatch2(string str, string pattern)
{
    int n = str.length();
    int m = pattern.length();

    if (m == 0 && n == 0)
        return true;

//    boolean[][] t = new boolean[n + 1][m + 1];
    vector<vector<bool>> t(n + 1, vector<bool>(m + 1, false));
    //		for(int i = 0; i < n + 1; i++)
    //			Arrays.fill(t[i], false);

    t[0][0] = true;

    for (int j = 1; j <= m; j++)
    {
        if (pattern.at(j - 1) == '*' || (pattern.at(j - 1) == '+'))
        {
            t[0][j] = t[0][j - 1];
        }
    }

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= m; j++)
        {
            if (pattern.at(j - 1) == '*')
            {
                t[i][j] = t[i][j - 1] || t[i - 1][j];
            }
            else if (pattern.at(j - 1) == '+')
            {
                t[i][j] = t[i][j - 1] || t[i - 1][j - 1];
            }
            else if (str.at(i - 1) == pattern.at(j - 1))
            {
                t[i][j] = t[i - 1][j - 1];
            }
            else
            {
                t[i][j] = false;
            }
        }
    }

    return t[n][m];
}
