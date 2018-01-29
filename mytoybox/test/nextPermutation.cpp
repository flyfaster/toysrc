#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class Solution {    // Runtime: 12 ms beat 47%
public:
    void nextPermutation(vector<int>& nums) {
        vector<int> desvec = nums;
        vector<int> ascvec = nums;
        sort(desvec.begin(), desvec.end(), std::greater<decltype(desvec[0])>());
        
        if (std::equal(nums.begin(), nums.end(), desvec.begin()))
        {
            sort(nums.begin(), nums.end());
            return;
        }
        for (int pos=nums.size()-1; pos>0; --pos)
        {
            if (nums[pos]>nums[pos-1])
            {
                int dd = nums[pos-1];
                sort(nums.begin()+pos-1, nums.end());
                for (int j=pos-1; j<nums.size(); ++j)
                {
                    if (nums[j]>dd)
                    {
                        swap(nums[pos-1], nums[j]);
                        sort(nums.begin()+pos, nums.end());
                        return;
                    }
                }
                
            }
        }
    }
};

// https://leetcode.com/problems/next-permutation/description/
int main()
{
    return 0;
}