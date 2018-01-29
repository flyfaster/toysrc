#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {    // 12ms beat 14%
        auto iter = lower_bound(nums.begin(), nums.end(), target);
        if (iter==nums.end()||*iter>target)
            return {-1, -1};
        auto iter2 = upper_bound(iter, nums.end(), target);
        if (iter2==nums.end())
            return {iter-nums.begin(), nums.size()-1};
        return {iter-nums.begin(), iter2-nums.begin()-1};
    }
    vector<int> searchRange1(vector<int>& nums, int target) {    // 12ms beat 14%
        auto range = equal_range(nums.begin(), nums.end(), target);
        if (range.first==range.second)
            return {-1,-1};
        int end = 0;
        if (range.second==nums.end())
            end = nums.size()-1;
        else
            end = range.second - nums.begin()-1;
        return {range.first-nums.begin(), end};
    }    
};

class Solution35 {
public:
    int searchInsert(vector<int>& nums, int target) {
        auto iter = lower_bound(nums.begin(), nums.end(), target);
        if (iter==nums.end())
            return nums.size();
        return iter-nums.begin();
    }
};

int main() {
    return 0;
}