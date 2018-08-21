#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <utility>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE insertion_sort
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

template<typename T>
void insertion_sort(T* s, size_t n ) {
	for (size_t i=1; i<n; ++i) {
		for (size_t j=i; j>0 && s[j]<s[j-1]; --j)
			std::swap(s[j], s[j-1]);
	}
}

using namespace std;

BOOST_AUTO_TEST_CASE(check_insertion_sort)
{
    cout << "BOOST_AUTO_TEST_CASE(check insertion sort.)" << endl;
    constexpr size_t n = 32768;
    int s[n];
    int s2[n];

    for (auto& d : s)
        d = rand();
    copy(begin(s), end(s), begin(s2));
    sort(begin(s), end(s));
    insertion_sort(s2, n);

    for (size_t i = 1; i < n; ++i)
    {
        BOOST_CHECK_EQUAL(s2[i], s[i]);
    }
}
