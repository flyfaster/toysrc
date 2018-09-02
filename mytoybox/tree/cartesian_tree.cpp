#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <vector>
#include <deque>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cartesian_tree
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;
using vi = vector<int>;

vi make_cartesian_tree(vi const& A)
{
    size_t n = A.size();
    vi parent(n, -1);
    stack<int> s;
    for (size_t i = 0; i < n; i++)
    {
        int last = -1;
        while (!s.empty() && A[s.top()] >= A[i])
        {
            last = s.top();
            s.pop();
        }
        if (!s.empty())
            parent[i] = s.top();
        if (last >= 0)
            parent[last] = i;
        s.push(i);
    }
    return parent;
}

deque<int> find_path(vi const& parent, int pos)
{
    deque<int> res;

    while (pos >=0)
    {
        res.push_front(pos);
        pos = parent[pos];
    }
    return res;
}

int find_lca(deque<int> const& p1, deque<int> const& p2)
{
    size_t len = min(p1.size(), p2.size());
    int res = p1[0];
    for (size_t i = 0; i < len; ++i)
    {
        if (p1[i] == p2[i])
            res = p1[i];
        else
            break;
    }
    return res;
}

BOOST_AUTO_TEST_CASE(check_bst)
{
    cout << "BOOST_AUTO_TEST_CASE(RMQ via LCA)" << endl;
    vi nums{6, 9, 2, 4, 7, 8, 5, 8, 3, 7};
    auto parent = make_cartesian_tree(nums);
    int left = 1;
    int right = 3;
    auto p1 = find_path(parent, left);
    auto p2 = find_path(parent, right);
    BOOST_TEST_CONTEXT("check Range Minimum Query(" << left << "-" << right << ")")
    BOOST_CHECK_EQUAL(2, find_lca(p1, p2));

    left = 0;
    right = nums.size() - 1;
    p1 = find_path(parent, left);
    p2 = find_path(parent, right);
    BOOST_TEST_CONTEXT("check Range Minimum Query(" << left << "-" << right << ")")
    BOOST_CHECK_EQUAL(2, find_lca(p1, p2));
}
