//============================================================================
// Name        : numSquares.cpp
//============================================================================

#include <iostream>
#include <cmath>
#include <set>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <deque>
#include <algorithm>
#include <unordered_map>
using namespace std;
#define VERBOSE
int verbose = 0;

class Solution {
public:
    void dbgOutput(const char* szFormat, ...)
	{
#ifdef VERBOSE
    	if(!verbose) return;
    	char szBuff[1024];
        va_list arg;
		va_start(arg, szFormat);
		vsnprintf (szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);
		std::cout << szBuff;
#endif
	}
	set<int> cache;  deque<int> cache2;
	unordered_map<int, int> numSquarescache;
	int subnum(int m, int left) {
//		dbgOutput("%s(%d,%d)\n", __func__, m, left);
		if(curmax <= 2) return curmax;
		if(m+1>=curmax) return curmax;
		{
			unordered_map<int, int>::iterator it = numSquarescache.find(left);
			if(it!=numSquarescache.end())
				return m+it->second;
		}

		int tmax = m+left;
		for(deque<int>::iterator it= lower_bound( cache2.begin(), cache2.end(), left); it!=cache2.end() ; it--)
		{
			auto square=*it;
			if(square>left) {
//				dbgOutput("(%d,square %d)\n",  left, square);
				continue;
			}
			if(m+1>=curmax) {
//				dbgOutput("(%d,curmax %d)\n",  m, curmax);
				continue;
			}
			int a = subnum(m+1, left - square);
			tmax = min(a, tmax);
//			dbgOutput("(tmax %d,square %d)\n",  tmax, square);
			if(square == 1) break;
		}
		if(numSquarescache.find(left)==numSquarescache.end() || numSquarescache[left]>tmax-m)
		{
//			dbgOutput("(%d,%d)\n",  left, tmax-m);
			numSquarescache[left]=tmax-m;
		}
		return curmax=min(tmax,curmax);
	}
	int curmax;
    int numSquares(int n) {
    	if(n<=1) return 1;
    	for(int i=1; i<n; i++) {
    		int tmp = i*i;
    		cache.insert(tmp);
    		cache2.push_back(tmp);
    		numSquarescache[tmp] = 1;
    		if(tmp<n/2) numSquarescache[tmp*2] = 2;
    		if(tmp > n)
    			break;

    	}
    	curmax = n;
    	return subnum(0, n);
    }
};

void test(int n) {
	Solution mysol;
	auto start= std::chrono::high_resolution_clock::now();
	int ret = mysol.numSquares(n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = end - start;
	std::chrono::milliseconds msduration = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	std::cout <<"numSquares("<<n <<") return " << ret << " used "<< msduration.count() <<"ms" << endl;
}
int main(int argc, char* argv[]) {
	cout << "perfect-squares" << endl;
	if (argc>2) verbose = atoi(argv[2]);
	if(argc>1) {
		int data = atoi(argv[1]);
		test(data);
		return 0;
	}
	test(12);
	test(6665);
	test(4703);
	test(8285);
	test(9975);
	return 0;
}

