#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

class Solution {
public:
    int search(const vector<int>& nums, int target, int start, int end) { 
        if (end-start<=32) {
            auto iter = find(nums.begin()+start, nums.begin()+end, target);
            return (iter!=nums.begin()+end)?iter-nums.begin():-1;
        }
        auto helper=[&nums](int target, int start, int mid)->size_t {
                auto iter=lower_bound(nums.begin()+start, nums.begin()+mid, target);
                if (iter==nums.begin()+mid || *iter!=target)
                    return -1;
                return iter-nums.begin();
        };
        int mid = end;
        if (nums[start]<nums[mid-1]) {
            return helper(target, start, mid);
        }
        mid = start + (end-start)/2;
        if (nums[start]<nums[mid-1]) {
            int ret = helper(target, start, mid);
            if (ret!=-1)
                return ret;
            return search(nums, target, mid, end);
        } else {
            int ret = helper(target, mid, end);
            if (ret!=-1)
                return ret;
            return search(nums, target, start, mid);            
        }
        return -1;
    }
    
    int search(vector<int>& nums, int target) { // 7ms beat 29%
        return search(nums, target, 0, nums.size());
    }
    
    int search2(const vector<int>& nums, int target, int start, int end) {

        auto helper=[&nums](int target, int start, int mid)->int {
            auto iter=lower_bound(nums.begin()+start, nums.begin()+mid, target);
            if (iter==nums.begin()+mid || *iter!=target)
                return -1;
            return iter-nums.begin();
        };
        auto helper2=[&nums, &helper, this](int target, int start, int mid, int start2, int mid2)->int {
            int ret = helper(target, start, mid);
            if (ret!=-1)
                return ret;
            return search(nums, target, start2, mid2);
        };
        
        int mid = end;
        
        if (mid-start<=32) {
            auto iter = find(nums.begin()+start, nums.begin()+mid, target);
            return (iter!=nums.begin()+mid)?iter-nums.begin():-1;
        }
        
        if (nums[start]<nums[mid-1]) 
            return helper(target, start, mid);
        
        mid = start + (end-start)/2;
        if (nums[start]<nums[mid-1])
            return helper2(target, start, mid, mid, end);
        return helper2(target, mid, end, start, mid);
    }
    
    int search1(const vector<int>& nums, int target, int start, int end) {
        if (end-start<=32) {
            auto iter = find(nums.begin()+start, nums.begin()+end, target);
            return (iter!=nums.begin()+end)?iter-nums.begin():-1;
        }
        auto helper=[&nums](int target, int start, int mid)->size_t {
                if (target<nums[start] || target>nums[mid-1])
                    return -1;
                auto iter=lower_bound(nums.begin()+start, nums.begin()+mid, target);
                if (iter==nums.begin()+mid || *iter!=target)
                    return -1;
                return iter-nums.begin();
        };
        int mid = end;
        if (nums[start]<nums[mid-1]) {
            return helper(target, start, mid);
        }
        mid = start + (end-start)/2;
        if (nums[start]<nums[mid-1]) {
            int ret = helper(target, start, mid);
            if (ret!=-1)
                return ret;
            return search(nums, target, mid, end);
        } else {
            int ret = helper(target, mid, end);
            if (ret!=-1)
                return ret;
            return search(nums, target, start, mid);            
        }
        return -1;
    }    
};

int main() {
    
    return 0;
}