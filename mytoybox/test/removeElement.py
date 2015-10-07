class Solution(object):
    def removeElement(self, nums, val):
        """
        :type nums: List[int]
        :type val: int
        :rtype: int
        """
        foundcnt=0
        for n, i in enumerate(nums):
            if i == val:
                foundcnt=foundcnt+1
            elif foundcnt >0:
                nums[n-foundcnt]=i
        return len(nums)-foundcnt
