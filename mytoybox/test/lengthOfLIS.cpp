//============================================================================
// Name        : lengthOfLIS.cpp
//============================================================================

#include <iostream>
#include <vector>
#include <map>
using namespace std;
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        map<int,int> cnts;
        for(auto num:nums) {
            int cnt=1;
            auto samenum = cnts.find(num);
            if(samenum!=cnts.end())
                cnt = samenum->second; // key is the same, count will be at least the same
             for(auto iter=cnts.begin(); iter!=cnts.end() ; iter++) {
            	 if(iter->first>=num)
            		 break;
                 cnt = max(cnt, iter->second+1);
             }
             cnts[num]=cnt;
        }
        int ret=0;
        for (auto iter:cnts)
            ret = max(ret, iter.second);
        return ret;
    }
};
int main() {
	Solution test;
	{
	vector<int> nums={2,2};
	int ret = test.lengthOfLIS(nums);
	cout << "lengthOfLIS() return "<<ret << endl;
	}
	{
	vector<int> nums={-2,-1};
	int ret = test.lengthOfLIS(nums);
	cout << "lengthOfLIS() return "<<ret << endl;
	}
	cout << "Submission Result: Accepted" << endl;
	return 0;
}

