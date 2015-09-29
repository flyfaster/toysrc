//============================================================================
// Name        : findDuplicate.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
using namespace std;
class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        int nmin = nums[0], nmax = nums[0];
        for(auto data: nums) {
            nmin = min(nmin, data);
            nmax = max(nmax, data);
        }
        int nmid = (nmin+nmax)/2;
        int lc=0, rc=0;
        int start=nmin, end=nmax;
        while(true) {
        	int nmid = (start+end)/2;
        	lc = rc = 0;
        	for(auto data: nums) {
        		if(data>nmid && data<=end) rc++;
        		if(data>=start && data<=nmid) lc++;
        	}
        	if(end-start<=1) {
//        		cout << "start="<<start<<",end="<<end<<",mid="<<nmid<<", lc="<<lc<<",rc="<<rc<<endl;
        		break;
        	}
        	if(lc>rc) end=nmid;
        	else start = nmid;
        }
        int ret = (lc>rc)?start:end;
        return ret;
    }
};
int main() {
	cout << "https://leetcode.com/problems/find-the-duplicate-number/" << endl;
	std::vector<int> input={1,2,5, 4,4,4,4};
	Solution mysol;
	cout << mysol.findDuplicate(input) << endl;
	input={1,3,4,2,1};
	cout << mysol.findDuplicate(input) << endl;
	return 0; // Submission Result: Accepted
}

