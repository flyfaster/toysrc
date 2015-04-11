#include <iostream>
#include <array>
#include <algorithm>

void siftDown(int *a, int k, int N) { // http://www.bogotobogo.com/Algorithms/heapsort.php
	while (k * 2 + 1 < N) {
		/* For zero-based arrays, the children are 2*i+1 and 2*i+2 */
		int child = 2 * k + 1; /* get bigger child if there are two children */
		if ((child + 1 < N) && (a[child] < a[child + 1]))
			child++;
		if (a[k] < a[child]) {
			/* out of max-heap order */
			std::swap(a[child], a[k]); /* repeat to continue sifting down the child now */
			k = child;
		} else
			return;
	}
}

void heapSort(int a[], int N) {
	/* heapify */
	for (int k = N / 2; k >= 0; k--) {
		siftDown(a, k, N);
	}
	while (N - 1 > 0) {
		/* swap the root(maximum value) of the heap with the last element of the heap */
		std::swap(a[N - 1], a[0]);
		/* put the heap back in max-heap order */
		siftDown(a, 0, N - 1); /* N-- : decrease the size of the heap by one so that the previous max value will stay in its proper placement */
		N--;
	}
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

static std::array<int, 64> unsortarr, sortarr1, sortarr2, sortarr3;
int main(int argc, char* argv[])
{
	std::cout << "test heapsort\n";
	std::cout <<"http://rosettacode.org/wiki/Sorting_algorithms/Heapsort#C.2B.2B"<<std::endl;
	for (int cnt = 0; cnt < 100; cnt++) {
		// verify 10 times, compare heapSort with C++ qsort function
		for (auto&& i : unsortarr) // http://en.cppreference.com/w/cpp/language/range-for
			i = rand() % (unsortarr.size() * 2);
		std::copy(unsortarr.begin(), unsortarr.end(), sortarr1.begin());
		std::copy(unsortarr.begin(), unsortarr.end(), sortarr2.begin());
		heapSort(sortarr1.data(), sortarr1.size());
		qsort(sortarr2.data(), sortarr2.size(), sizeof(int), compare);
		for (int i = 0; i < sortarr1.size(); i++) {
			if (sortarr1[i] != sortarr2[i])
				std::cout << "unmatch index " << i << ", heapSort[i] "
						<< sortarr1[i] << ", qsort[i] " << sortarr2[i]
						<< std::endl;
		}
	}
	std::cout << argv[0] << " done\n";
}
