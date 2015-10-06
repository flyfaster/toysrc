class Solution(object):
    List=[]
    dict={}
    def comb(self, prefix, digits):
        if len(digits) == 0:
            self.List.append(prefix)
        elif len(digits) == 1:
            for letter in self.dict[digits]:
                self.List.append( prefix+letter)
        else:
            first = digits[0]
            rest = digits[1:]
            for letter in self.dict[first]:
                self.comb( prefix+letter, rest)
                
    def letterCombinations(self, digits):
        """
        :type digits: str
        :rtype: List[str]
        """
        self.List=[]
        if len(digits) == 0:
            return []
        self.dict={'1':'','2':'abc','3':'def','4':'ghi','5':'jkl','6':'mno','7':'pqrs','8':'tuv','9':'wxyz','0':' '}
        self.comb( '',digits)
        return self.List
        
if  __name__ =='__main__':
	mysol = Solution()
	ret = mysol.letterCombinations('3')
	for a in ret:
		print(a)        
