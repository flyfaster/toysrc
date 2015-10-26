class Solution(object):
    def countDna(self, dnaset, dna):
        if dna in dnaset:
            dnaset[dna] = dnaset[dna]+1
            return True
        else:
            dnaset[dna] = 0
            return False
    def findRepeatedDnaSequences(self, s):
        """
        :type s: str
        :rtype: List[str]
        """
        ret=[]
        dnaset={}
        for i in range(0, len(s)-9):
            dna=s[i:i+10]
            if dna in dnaset:
                dnaset[dna]=dnaset[dna]+1
                continue
            dnaset[dna]=0
        for dna,cnt in dnaset.iteritems():
            if cnt>0:
                ret.append(dna)
        return ret 
if __name__ == "__main__":
    test=Solution()
    ret = test.findRepeatedDnaSequences("AAAAACCCCCAAAAACCCCCCAAAAAGGGTTT")
    print(ret) 
    ret = test.findRepeatedDnaSequences("AAAAAAAAAAA")
    print(ret)        # Submission Result: Accepted
    
