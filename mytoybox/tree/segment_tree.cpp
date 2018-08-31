#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE segment_tree
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;

class segment_tree
{
    vector<int> m_st;
	vector<int> m_raw_data;
    int left(int p)
    {
        return p << 1;
    }

    int right(int p)
    {
        return (p << 1) + 1;
    }

    void build(int p, int L, int R)
    {
    	maxp = max(maxp, p);
        if (L == R)	// single element range
            m_st[p] = L;
        else
        {
            build(left(p), L, (L + R) / 2);
            build(right(p), (L + R) / 2 + 1, R);
            int p1 = m_st[left(p)];
            int p2 = m_st[right(p)];
            m_st[p] = (m_raw_data[p1] <= m_raw_data[p2]) ? p1 : p2;
        }
    }

    int out_of_range = -1;

    int rmq(int p, int L, int R, int qleft, int qright)
    {
        if (qleft > R || qright < L) // current segment outside query range
            return out_of_range;
        if (L >= qleft && R <= qright) // inside query range
            return m_st[p];
        int p1 = rmq(left(p), L, (L + R) / 2, qleft, qright);
        int p2 = rmq(right(p), (L + R) / 2 + 1, R, qleft, qright);
        if (p1 == out_of_range)
            return p2;
        if (p2 == out_of_range)
            return p1;
        return (m_raw_data[p1] < m_raw_data[p2]) ? p1 : p2;
    }

public:
    segment_tree(const vector<int>& src)
    {
        m_st.assign(src.size() * 2+1, 0);
        m_raw_data = src;
        build(1, 0, src.size() - 1); // root is at 1
    }

    int rmq(int qleft, int qright)
    {
        return rmq(1, 0, m_raw_data.size() - 1, qleft, qright);
    }
    int maxp = 0;
};

BOOST_AUTO_TEST_CASE(check_segment_tree)
{
    cout << "BOOST_AUTO_TEST_CASE(segment_tree)" << endl;
    vector<int> A{18, 17, 13, 19, 15, 11, 20, 1};
    segment_tree st(A);

    BOOST_CHECK_EQUAL(2, st.rmq(1, 3));
    BOOST_CHECK_EQUAL(5, st.rmq(4, 6));
    BOOST_CHECK_EQUAL(7, st.rmq(2, 7));
    cout << "maxp is " << st.maxp << "\n";
}
