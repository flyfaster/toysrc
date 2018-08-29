#include <vector>
#include <algorithm>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE backtrack
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

void queens(vector<int>& row, vector<int>& diag2, vector<int>& diag, size_t y,
            size_t& cnt)
{
    if (y == row.size())
    {
        ++cnt;
        return;
    }
    for (size_t x = 0; x < row.size(); ++x)
    {
        size_t d1 = row.size() - 1 + x - y;
        if (row[x] || diag2[d1] || diag[x + y])
            continue;
        row[x] = diag2[d1] = diag[x + y] = true;
        queens(row, diag2, diag, y + 1, cnt);
        row[x] = diag2[d1] = diag[x + y] = false;
    }
}

BOOST_AUTO_TEST_CASE(check_queens)
{
    cout << "BOOST_AUTO_TEST_CASE(counting the number of ways to place queens.)"
         << endl;
    {
        size_t board_size = 4;
        vector<int> row(board_size, 0), col(board_size * 2, 0), diag(board_size * 2, 0);
        size_t cnt = 0;
        queens(row, col, diag, 0, cnt);
        BOOST_TEST_CONTEXT("queens(" << board_size << ")")
        BOOST_CHECK_EQUAL(cnt, 2);
    }
    {
        size_t board_size = 8;
        vector<int> row(board_size, 0), col(board_size * 2, 0), diag(board_size * 2, 0);
        size_t cnt = 0;
        queens(row, col, diag, 0, cnt);
        BOOST_TEST_CONTEXT("queens(" << board_size << ")")
        BOOST_CHECK_EQUAL(cnt, 92);
    }
}

