import sys
           
class Solution(object):
    def generateParenthesis(self, n):
        """
        :type n: int
        :rtype: List[str]
        """
        List=[]   
        'print("n is {}".format(n))  '   
        if n<1:
            List.append('')
            return List 
        if n==1:
            List.append("()")
            return List 
        for i in range(0, n):
            for left in self.generateParenthesis(i):
                for right in self.generateParenthesis(n-1-i):
                    List.append('('+left+')'+right)
        'print("list cnt is {}".format(len(List)))'
        return List
                
if __name__ == "__main__":
    mysol = Solution()
    if len(sys.argv)>1:
        ret=mysol.generateParenthesis(int(sys.argv[1]))
        print("numer of combinations is {}".format(len(ret)))
        for item in ret:
            print(item, end=' ')
    else:
        print("enter a number ")
