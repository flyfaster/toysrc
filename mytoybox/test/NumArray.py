#!/usr/bin/python
#from __future__ import print_function
# def print(*args, **kwargs):
#     """My custom print() function."""
#     # Adding new arguments to the print function signature 
#     # is probably a bad idea.
#     # Instead consider testing if custom argument keywords
#     # are present in kwargs
#     return
#     __builtins__.print('My overridden print() function!')
#     return __builtins__.print(*args, **kwargs)
import math, sys, copy
class NumArray(object):  #Submission Result: Accepted
    def myprint(self, msg):
        print(msg)
        pass
    def sum8(self, nums1, nums2, count):
        tmp = 0
        index = 0
        for i in range(0, len(nums1)):
            tmp = tmp+nums1[i]
            if i%count == count-1:
                nums2.append(tmp)
                tmp=0
                index=index+1
        if (len(nums1)) % count >0:
            nums2.append(tmp)
            
    def __init__(self, nums):
        """
        initialize your data structure here.
        :type nums: List[int]
        """
        self.sums = []
        cnt=8
        ind=1
        self.sums.append(copy.deepcopy(nums))
        while cnt <= len(nums):
            cnt = cnt*8
            ind = ind+1
            self.sums.append([])
        self.sums.append([]) # an extra list
#         ind = 0
        for i in range(0, ind):
            if len(self.sums[i]) < 8:
                break
            self.sum8(self.sums[i], self.sums[i+1], 8)
    def getpos(self, k, count):
        ret = int((k)/count)
        return ret
    def update(self, k, val):
        """
        :type i: int
        :type val: int
        :rtype: int
        """
        detaval = val- self.sums[0][k]
#         self.myprint("k is {} val is {} diff is {}".format(k, val, detaval))
        if detaval != 0:
            self.sums[0][k] = val
            ind = 1
            count=8
            while len(self.sums[0])>=count:
#                 if k >9:
#                     self.myprint("ind is {} count is {} k is {}, pos is {}".format(ind, count, k, self.getpos(k,count)))
                self.sums[ind][self.getpos(k,count)] += detaval
                count = count*8
                ind +=1

    def sumRange2(self, i):
        ret = 0
        ind=1
        count=8
        pos=0
        while pos<=i:
            while count<=i-pos:
                count=count*8
                ind = ind +1
            while count>8 and count>i-pos:
                count = count/8
                ind = ind-1
#             if (i-pos)/count>0:
#                 ret = ret + self.sums[ind][(i-pos)/count-1]
            loopcnt = (i-pos)/count
            for k in range(0, loopcnt):
                ret = ret + self.sums[ind][pos/count]
                pos = pos + count
#             self.myprint("pos is {} count is {} i is {} loopcnt is {} ind is {}".format(pos, count, i, loopcnt, ind))
            if i>=pos and (i-pos)<8:
                for k in range(pos, i+1):
#                     if i>=8944:
#                         self.myprint("pos is {} k is {} i is {}".format(pos, k, i))
                    ret = ret + self.sums[0][k]
                break
        return ret
    def slowSum(self, i):
        sum1=0
        for k in range(0, i+1):        
            sum1 += self.sums[0][k] 
        return sum1

    def sumRange(self, i, j):
        """
        sum of elements nums[i..j], inclusive.
        :type i: int
        :type j: int
        :rtype: int
        """
        ret = 0
        ret = self.sumRange2(j) - self.sumRange2(i)+self.sums[0][i]
        return ret
    def dump(self):
        for li in self.sums:
            print(li)
          
def test4(filename):
    srcfile=open(filename)
    content = srcfile.read()
    leftb = content.find("[")
    rightb = content.find("]")
    numstr = content[leftb+1:rightb]
    strlist = numstr.split(",")
    nums = []
    for i in strlist:
        nums.append(int(i))
    test = NumArray(nums)
#     print("sum of {} to {} is {}".format(451,14231,test.sumRange(451,14231)) )
    targetpos = 8944
    print("sum to {} is {} expect {}".format( targetpos, test.sumRange2(targetpos), test.slowSum(targetpos)) )
    test.update(8943,9)
    print("after update sum to{} is {} expect {}".format( targetpos, test.sumRange2(targetpos), test.slowSum(targetpos)) )
    for i in range(16, len(nums)) :
        s1 = test.sumRange2(i)
        s2 = test.slowSum(i)
        if s1 != s2:
            print("Sum to {} expect {} got {}".format(i, s2, s1))
            break
    for i in range(8943,8945):
        print("{}th num is {}".format(i, test.sums[0][i]))
if __name__ == "__main__":
#     test3()
    if len(sys.argv)>1:
        srcfile = sys.argv[1]
        test4(srcfile)
        sys.exit(0)
    
def test1():    
    nums = [-28,-39,53,65,11,-56,-65,-39,-43,97]
    test = NumArray(nums)
    test.sumRange(5,6)
    test.update(9,27) 
    test.sumRange(2,3) 
    test.sumRange(6,7) 
    test.update(1,-82) 
    test.update(3,-72) 
    test.sumRange(3,7) 
    test.sumRange(1,8) 
    test.update(5,13) 
    test.update(4,-67)

def test2():
    nums =[1,3,5] 
    test = NumArray(nums) 
    test.sumRange(0,2) 
    test.update(1,2) 
    test.sumRange(0,2)
    
def test3():
    nums = [-28,-39,53,65,11,-56,-65,-39,-43,97] 
    test = NumArray(nums) 
    test.sumRange(5,6),
    test.update(9,27),
    test.sumRange(2,3),
    test.sumRange(6,7),
    test.update(1,-82),
    test.update(3,-72),
    test.sumRange(3,7),
    print("last result is {}".format(test.sumRange(1,8)))
    print("sum to 1 is {}, sum to 8 is {}".format(test.sumRange2(1), test.sumRange2(8)))
    test.dump()
    test.update(5,13),
    test.update(4,-67)

# Definition for a  binary tree node
class TreeNode:
    def __init__(self, x):
        self.val = x
        self.left = None
        self.right = None
        
class BinaryTree:
    # @param num, a list of integers
    # @return a tree node
    def sortedArrayToBST(self, num):
        return self.sortedArrayToBSTRec(num, 0, len(num)-1)
        
    def sortedArrayToBSTRec(self, num, begin, end):
        if begin>end:
            return None
        midPoint = (begin+end)//2
        root = TreeNode(num[midPoint])
        root.left = self.sortedArrayToBSTRec(num, begin, midPoint-1)
        root.right = self.sortedArrayToBSTRec(num, midPoint+1,end)
        return root
