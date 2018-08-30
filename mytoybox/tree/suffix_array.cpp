#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#include <numeric>
#include <string>
#include "boost/optional.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE suffix_array
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;
using boost::optional;

struct suffix_array_cmp { // functors-for-comparison
	suffix_array_cmp(const char* str) : m_str(str){};
	const char* m_str;
	bool operator()(int a, int b) const {
		return strcmp(m_str+a, m_str+b)<0; // O(n)
	}
};

vector<int> create_suffix_array(const char* str)
{
    vector<int> sa(strlen(str) + 1);
    iota(begin(sa), end(sa), 0);
    suffix_array_cmp cmp(str);
    sort(begin(sa), end(sa), cmp); // O(n^2 log n)
    for (int i = 0; i < sa.size(); ++i)
        cout <<i << "\t" << str + i << "\n";
    return sa;
}

BOOST_AUTO_TEST_CASE(check_suffix_array)
{
    cout << "BOOST_AUTO_TEST_CASE(check_suffix_array)" << endl;
    auto sa = create_suffix_array("GATAGACA");
    BOOST_CHECK_EQUAL(9, sa.size());
}
