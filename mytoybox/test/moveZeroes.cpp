//============================================================================
// Name        : moveZeroes.cpp
//============================================================================

#include <iostream>
#include <vector>
#include <stdarg.h>
#include <cstdio>
#define VERBOSE
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
    void fixsub(vector<int>& nums, int start, int end) {
        if(end-start>2) {
            int mid = start+(end-start)/2;
            fixsub(nums, start, mid);
            fixsub(nums, mid, end);
            int i=start; int j=mid;
            while(nums[i] && i<mid) {
            	dbgOutput("nums[%d]=%d\n", i, nums[i]);
            	i++;
            }
            if(i==mid) return; // already sorted
            while(nums[j] && j<end) j++;
            if(j==mid) return; // [mid,end] are all 0 already sorted
            dbgOutput("swap %d and %d, nums[i]=%d nums[j]=%d, mid=%d\n", i, j, nums[i], nums[j], mid);
            for(int k=0; k<j-mid; k++) {
            	dbgOutput("swap %d and %d\n", i+k, mid+k);
                swap(nums[i+k], nums[mid+k]);
            }
            return;
        }
        if(end-start==2 && nums[start]==0 && nums[start+1]){
            swap(nums[start], nums[start+1]);
        }
    }
    void moveZeroes(vector<int>& nums) {
        fixsub(nums,0, nums.size());
    }
};
int main() {
	cout << "move-zeroes" << endl;
	Solution mysol;
	vector<int> nums={1,2,3,1};
	mysol.moveZeroes(nums);
	for(auto data: nums)
		cout << data << " ";
	cout << endl;
	return 0;
}

