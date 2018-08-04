#include <iostream>
#include <array>
#include <iterator>
#include <vector>
#include <boost/timer/timer.hpp>
using namespace std;

template<typename I>
void doMerge1(I begin, I midPoint, I end)
{
    typedef std::vector<typename std::iterator_traits<I>::value_type> TmpVec;
TmpVec  tmp(std::make_move_iterator(begin), std::make_move_iterator(end));
typename TmpVec::iterator   beginAlt   = std::begin(tmp);
typename    TmpVec::iterator   endAlt     = std::end(tmp);
typename    TmpVec::iterator   midAlt     = std::next(beginAlt, std::distance(begin, midPoint));
typename    TmpVec::iterator   l   = beginAlt;
typename    TmpVec::iterator   r   = midAlt;
    I                  i   = begin;
    while(l < midAlt && r < endAlt)
    {
        *i = std::move((*l < *r) ? *l++ : *r++);
        ++i;
    }
    while(l < midAlt)
    {   *i  = std::move(*l++);
        ++i;
    }
    while(r < endAlt)
    {   *i  = std::move(*r++);
        ++i;
    }
}

template<typename I>
void doMerge(I begin, I midPoint, I end)
{
//    typedef std::vector<typename std::iterator_traits<I>::value_type> TmpVec;
//TmpVec  tmp(std::make_move_iterator(begin), std::make_move_iterator(end));
//typename TmpVec::iterator   beginAlt   = std::begin(tmp);
//typename    TmpVec::iterator   endAlt     = std::end(tmp);
//typename    TmpVec::iterator   midAlt     = std::next(beginAlt, std::distance(begin, midPoint));
//typename    TmpVec::iterator   l   = beginAlt;
//typename    TmpVec::iterator   r   = midAlt;
	I r = midPoint;
	I l = begin;
	std::size_t length = std::distance(midPoint, end);
	I rbound = std::next(midPoint, length-1);
//    I                  i   = begin;
    while(l < rbound && r < end)
    {
        if (*l<=*r)
        {
        }
        else
        {
//        	typename std::iterator_traits<I>::value_type tmp;
//        	tmp = std::move(*r);
        	std::swap(*l, *r);
        	if (rbound!=r)
        		r++;
        }
        l++;
//        if (l==r)
//        	r++;
    }
}
template<typename I>
void mergeSort(I begin, I end) { // http://codereview.stackexchange.com/questions/49459/merge-sort-algorithm-implementation-using-c
	std::size_t length = std::distance(begin, end);
	if (length <= 1) {
		return;
	}
	if (length==2)
	{
		I left = begin;
		I right = std::next(begin, 1);
		if (*left>*right)
		{
//			std::cout << "before:" << *begin<< " " << *right <<std::endl;
			std::swap(*left, *right);
//			std::cout << "after:" << *begin<< " " << *right <<std::endl;
		}
		return;
	}
	std::size_t mid = length / 2;
	I midPoint = std::next(begin, mid);
	mergeSort(begin, midPoint);
	mergeSort(midPoint, end);
	doMerge(begin, midPoint, end);
}

void testmerge()
{
	std::cout<<"not working!!!!!!!!!!!!!!!!\n";
	std::array<int, 16> unsortarr, sortarr1, sortarr2, sortarr3;

	//	doMerge(test.begin(), std::next(test.begin(), test.size()/2), test.end());
	//	for (auto data: test)
	//		std::cout << data << std::endl;
	//	return 0;
	{
//		boost::timer::auto_cpu_timer t;
		mergeSort(std::begin(sortarr3), std::end(sortarr3));
	}
	cout << "mergeSort done\n";
}

//////////////// http://rosettacode.org/wiki/Sorting_algorithms/Merge_sort#C.2B.2B
#include <iterator>
#include <algorithm> // for std::inplace_merge
#include <functional> // for std::less

template<typename RandomAccessIterator, typename Order>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last, Order order)
{
  if (last - first > 1)
  {
    RandomAccessIterator middle = first + (last - first) / 2;
    mergesort(first, middle, order);
    mergesort(middle, last, order);
    std::inplace_merge(first, middle, last, order);
  }
}

template<typename RandomAccessIterator>
 void mergesort(RandomAccessIterator first, RandomAccessIterator last)
{
  mergesort(first, last, std::less<typename std::iterator_traits<RandomAccessIterator>::value_type>());
}

void merge (int *a, int n, int m) {
    int i, j, k;
    int *x = (int*)malloc(n * sizeof (int));
    for (i = 0, j = m, k = 0; k < n; k++) {
        x[k] = j == n      ? a[i++]
             : i == m      ? a[j++]
             : a[j] < a[i] ? a[j++]
             :               a[i++];
    }
    for (i = 0; i < n; i++) {
        a[i] = x[i];
    }
    free(x);
}

void merge_sort (int *a, int n) {
    if (n < 2)
        return;
    int m = n / 2;
    merge_sort(a, m);
    merge_sort(a + m, n - m);
    merge(a, n, m);
}

template<typename T,size_t N>
int compare_array(std::array<T, N>& left, std::array<T,N>& right) {
	int unmatchcnt=0;
	for (int i = 0; i < left.size(); i++) {
		if (left[i] != right[i]) {
			unmatchcnt++;
			std::cout << "unmatch index " << i << ", left[i] "
					<< left[i] << ", right[i] " << right[i]
					<< std::endl;
		}
	}
	return unmatchcnt;
}
int main(int argc, char* argv[])
{
	std::cout << "http://rosettacode.org/wiki/Sorting_algorithms/Merge_sort#C.2B.2B" <<std::endl;
	std::cout << "g++ -o mergesort.exe -std=c++11 mergesort.cpp" << std::endl;
	std::array<int, 64> unsortarr, sortarr1, sortarr2, sortarr3;
	for (auto&& i : unsortarr) // http://en.cppreference.com/w/cpp/language/range-for
		i = rand() % (unsortarr.size() * 2);
	std::copy(unsortarr.begin(), unsortarr.end(), sortarr1.begin());
	std::copy(unsortarr.begin(), unsortarr.end(), sortarr2.begin());
	std::copy(unsortarr.begin(), unsortarr.end(), sortarr3.begin());
	mergesort(sortarr1.begin(), sortarr1.end() );
	std::sort(sortarr2.begin(), sortarr2.end());
	merge_sort(sortarr3.data(), sortarr3.size());

	compare_array(sortarr1, sortarr2);
	compare_array(sortarr2, sortarr3);
	std::cout <<argv[0] << " done\n";
	return 0;
}
