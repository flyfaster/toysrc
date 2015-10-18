class Solution(object):
    def rob(self, nums):
        """
        :type nums: List[int]
        :rtype: int
	Submission Accepted
        """
        if len(nums) <1:
            return 0
        if len(nums) == 1:
            return nums[0]
        if len(nums) == 2:
            return max(nums)
        a=nums[0]
        b=max(a, nums[1])
        cur=max(a,b)
        for i in range(2, len(nums)):
            cur=max(a+nums[i],b)
            a=b 
            b=cur
        return cur
