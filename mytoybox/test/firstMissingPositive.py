class Solution(object):
    def firstMissingPositive(self, nums):
        """
        :type nums: List[int]
        :rtype: int
        """
        tmp = 0
        for i in range(0, len(nums)):
            data = nums[i]
            while data >0 and data <= len(nums) and nums[data-1] != data :
                tmp = nums[data-1]
                nums[data-1] = data 
                nums[i] = tmp 
                data = tmp
#         for data in nums:
#             print(data, end=' ')
#         print(" ")
        for i in range(0, len(nums)):
            if nums[i] != i+1:
                return i+1 
        return len(nums)+1
if __name__ == "__main__":
    test=Solution()
    ret = test.firstMissingPositive([3,4,-1,1])
    print("return {}".format(ret))
    print("Submission Result: Accepted ")            
