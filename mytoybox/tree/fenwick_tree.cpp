#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE fenwick_tree
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;

class fenwick_tree
{
    vector<int> ft;

public:
    fenwick_tree(int n) : ft(n + 1, 0)
    {
    }
    int rsq(int b)
    {
        int sum = 0;
        for (; b; b -= least_significant_bit(b))
            sum += ft[b];
        return sum;
    }
    int rsq(int a, int b)
    {
        return rsq(b) - (a == 1 ? 0 : rsq(a - 1));
    }
    void adjust(int k, int v)
    {
        for (; k < ft.size(); k += least_significant_bit(k))
            ft[k] += v;
    }
    int least_significant_bit(int b)
    {
        return b & ~(b - 1);
    }
};

BOOST_AUTO_TEST_CASE(check_fenwick_tree)
{
    cout << "BOOST_AUTO_TEST_CASE(fenwick_tree)" << endl;
    vector<int> A{2, 4, 5, 5, 6, 6, 6, 7, 7, 8, 9};
    fenwick_tree tree(A.size());
    for (int i = 0; i < A.size(); i++)
        tree.adjust(A[i], 1);

    BOOST_CHECK_EQUAL(0, tree.rsq(1, 1));
    BOOST_CHECK_EQUAL(1, tree.rsq(1, 2));
    BOOST_CHECK_EQUAL(7, tree.rsq(1, 6));
}
