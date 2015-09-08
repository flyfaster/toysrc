//============================================================================
// Name        : findMedianSortedArrays.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
// Description : //There are two sorted arrays nums1 and nums2 of size m and n respectively. 
//               Find the median of the two sorted arrays.
//               The overall run time complexity should be O(log (m+n)).
//============================================================================

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdarg.h>
#include <stdio.h>
using namespace std;
class Solution {
public:
    void dbgOutput(const char* szFormat, ...)
	{
		char szBuff[1024];
#ifdef VERBOSE
        va_list arg;
		va_start(arg, szFormat);
		vsnprintf (szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);
		std::cout << szBuff;
#endif
	}
	double getMedian(vector<int>& nums) {
		int mid = nums.size()/2;
		if(nums.size()%2)
			return nums[mid];
		return ((double)nums[mid]+(double)nums[mid-1])/2.0;
	}
	size_t getStep(vector<int>& nums, size_t pos, size_t gap) {
		size_t newpos = gap/2+pos;
		if(newpos>=nums.size())
		{
			newpos = nums.size()-1;
		}
		dbgOutput("%s vecotr size %zd current pos %zd gap %zd return %zd\n", __FUNCTION__, nums.size(), pos, gap, newpos);
		return newpos;
	}

    int findSortedArrays(vector<int>& nums1, size_t pos1, vector<int>& nums2, size_t pos2, size_t target) {
    	dbgOutput("%s nums1[%zd] nums2[%zd] look for %zdth element\n", __FUNCTION__, pos1, pos2, target);
    	if(pos1<0 || pos2<0)
    		return 0;
    	size_t gap = target - pos1 -pos2;
    	if(nums1.size()<=pos1) {
    		dbgOutput("%s return element from nums2 at %zd\n", __FUNCTION__, pos2+gap);
    		return nums2[pos2+gap];
    	}
    	if(nums2.size()<=pos2) {
    		dbgOutput("%s return element from nums1 at %zd\n", __FUNCTION__, pos1+gap);
    		return nums1[pos1+gap];
    	}

    	size_t step1 = getStep(nums1, pos1, gap);
    	size_t step2 = getStep(nums2, pos2, gap);
    	int ret = std::min(nums1[pos1], nums2[pos2]);
    	if(nums1[step1]<=nums2[step2]) {
    		if (step1==pos1) pos1++; else pos1 = step1;
    	} else {
    		if (step2==pos2) pos2++; else pos2 = step2;
    	}
    	if (pos1+pos2>target) {
    		dbgOutput("%s nums1[%zd] nums2[%zd] found %zdth element %d\n", __FUNCTION__, pos1, pos2, target, ret);
    		return ret;
    	}
    	return findSortedArrays(nums1, pos1, nums2, pos2, target);
    }
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
    	if(nums1.size()==0) return getMedian(nums2);
    	if(nums2.size()==0) return getMedian(nums1);
    	size_t midpos = (nums1.size()+nums2.size())/2;
    	dbgOutput("%s midpos is %zd\n", __FUNCTION__, midpos);
    	if( (nums1.size()+nums2.size()) %2==0) {
    		return ((double)(findSortedArrays(nums1, 0, nums2, 0, midpos-1)+findSortedArrays(nums1, 0, nums2, 0, midpos)))/2.0;
    	}
    	return findSortedArrays(nums1, 0, nums2, 0, midpos);
    }
};
int main() {
	cout << "https://leetcode.com/problems/median-of-two-sorted-arrays/" << endl;
	Solution mysol;
//	if (true) {
//	std::vector<int> nums1={1};
//	std::vector<int> nums2={1};
//	cout << "answer " << mysol.findMedianSortedArrays(nums1, nums2) << endl;
//	}
//	if (true) {
//	std::vector<int> nums1={1};
//	std::vector<int> nums2={2,3,4};
//	cout << "answer " << mysol.findMedianSortedArrays(nums1, nums2) << endl;
//	}
	if (true) {
	std::vector<int> nums1={1,2};
	std::vector<int> nums2={1,2};
	cout << "answer " << mysol.findMedianSortedArrays(nums1, nums2) << endl;
	}
	return 0;
}


