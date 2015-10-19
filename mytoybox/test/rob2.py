class Solution(object):
    def rob1(self, nums):
        """
        :type nums: List[int]
        :rtype: int
        Submission Result: Accepted
        """
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
    def rob2(self, nums):
        """
        After robbing those houses on that street, the thief has found 
        himself a new place for his thievery so that he will not get too much 
        attention. This time, all houses at this place are arranged in a circle. 
        That means the first house is the neighbor of the last one. Meanwhile, 
        the security system for these houses remain the same as for those in the previous street.
        Given a list of non-negative integers representing the amount of money of each house, 
        determine the maximum amount of money you can rob tonight without alerting the police.
        """
        if len(nums) <1:
            return 0
        if len(nums)<4:
            return max(nums)
        ret1=self.rob1(nums[1:])
        ret2=self.rob1(nums[:-1])
        if ret1>ret2:
            return ret1
        return ret2
    def rob(self, nums):
        return self.rob2(nums)
if __name__ == "__main__":
    test = Solution()
    ret=test.rob([])
    print("return {}".format(ret))    
    print("Submission Result: Accepted")
