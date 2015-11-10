class NumArray(object):
    def __init__(self, nums):
        """
        initialize your data structure here.
        :type nums: List[int]
        """
        self.sum_of_index=[0]
        lastsum=0
        for i in range(0, len(nums)):
            self.sum_of_index.append(lastsum+nums[i])
            lastsum=lastsum+nums[i]

    def sumRange(self, i, j):
        """
        sum of elements nums[i..j], inclusive.
        :type i: int
        :type j: int
        :rtype: int
        """
        ret = self.sum_of_index[j+1]-self.sum_of_index[i]
        return ret

#Submission Result: Accepted
