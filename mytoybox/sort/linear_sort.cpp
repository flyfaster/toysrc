#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>
#include <type_traits>
#include <numeric>
#include <utility>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE linear_sort
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;
/*
A: input array
B: output array to store sorted data
k: size of universe

COUNTING-SORT(A; B; k)
1 size_t C[k]{}; // be a new array to store element count
2
3
4 for j = 1 to A.length
5 	++C[A[j]];
6 // C[i] now contains the number of elements equal to i.
7 for i = 1 to k
8 	C[i] = C[i] + C[i-1]
9 // C[i] now contains the number of elements less than or equal to i.
10 for j = A.length down to 1
11 	B[C[A[j]]] = A[j]	// stable sort
12 	--C[A[j]];
*/
void counting_sort(const vector<int>& src, vector<int>& dst, int universe)
{
    vector<int> counters(universe + 1, 0);
    for (auto data : src)
        ++counters[data];
    std::partial_sum(counters.begin(), counters.end(), counters.begin());
    for (int i = src.size() - 1; i >= 0; --i)
    {
        --counters[src[i]];
        dst[counters[src[i]]] = src[i];
    }
}

BOOST_AUTO_TEST_CASE(check_counting_sort)
{
    cout << "BOOST_AUTO_TEST_CASE(check counting_sort.)" << endl;
    vector<int> src;
    int universe = 1024 * 64;
    while (src.size() < 8192)
        src.push_back(rand() % universe);
    vector<int> out1(src.size());
    counting_sort(src, out1, universe);

    vector<int> out2;
    out2.assign(src.begin(), src.end());
    sort(out2.begin(), out2.end());

    for (size_t i = 0; i < src.size(); ++i)
    {
        BOOST_TEST_CONTEXT("check_counting_sort(" << i << ")")
        BOOST_CHECK_EQUAL(out2[i], out1[i]);
    }
}

/*
RADIX-SORT(A, d)
1 for i = 1 to d
2 	use a stable sort to sort array A on digit i
*/

/*
bucket-sort(A)	// assume A is uniformly distributed in [0,1)
	n = A.length
	list<float> B[n];
	for i=1 to n
		insert A[i] to B[n*A[i]]
	for i=0 to n-1
		sort B[i] with insertion sort
	concatenate the lists B[0],...,B[n-1] together in order

*/



BOOST_AUTO_TEST_CASE(check_linear_sort)
{
    cout << "BOOST_AUTO_TEST_CASE(check insertion sort.)" << endl;
    size_t src[]={0, 3, 1, 0, 4, 0};
    size_t out1[std::extent<decltype(src)>::value];
    size_t out2[std::extent<decltype(src)>::value];
    copy(begin(src), end(src), begin(out1));
    copy(begin(src), end(src), begin(out2));

    for(size_t i=1; i< std::extent<decltype(src)>::value; ++i)
    	out1[i] += out1[i-1];

    std::partial_sum(begin(out2), end(out2), begin(out2));
    for (size_t i = 1; i < std::extent<decltype(src)>::value; ++i)
    {
    	BOOST_TEST_CONTEXT("partial_sum(" << i << ")")
        BOOST_CHECK_EQUAL(out2[i], out1[i]);
    }
}
