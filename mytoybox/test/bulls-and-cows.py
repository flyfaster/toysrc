import sys, math
class Solution(object):
    def getHint(self, secret, guess):
        """
        :type secret: str
        :type guess: str
        :rtype: str
        """
        bulls=0
        cows=0
        srccnt={}
        gcnt={}
        for i in range(0, len(secret)):
            if secret[i] not in srccnt:
                srccnt[secret[i]]=0
            srccnt[secret[i]]=srccnt[secret[i]]+1
        for i in range(0, len(secret)):
            if guess[i] in srccnt:
                if guess[i] not in gcnt:
                    gcnt[guess[i]] = 0
                gcnt[guess[i]] = gcnt[guess[i]]+1
            if secret[i] == guess[i]:
                bulls = bulls+1
#         for k,v in srccnt.iteritems():
#             if k in gcnt:
#                 if v > gcnt[k]:
#                     cows = cows + v - gcnt[k]
        for k, v in gcnt.iteritems():
            cows = cows + min(v, srccnt[k])
        cows = cows - bulls
        #print("cows {} bulls {}".format(cows, bulls))
        ret = "{0}A{1}B".format(bulls,cows)
        return ret
if __name__ == "__main__":
    test=Solution()
    if len(sys.argv) >2:
        print("Solution return {}".format(test.getHint(sys.argv[1], sys.argv[2])))
    print("Solution return {}".format(test.getHint("11", "10")))
#    Submission Result: Accepted
