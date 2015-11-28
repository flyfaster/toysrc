class Codec: #Submission Result: Accepted

    def encode(self, strs):
        """Encodes a list of strings to a single string.
        
        :type strs: List[str]
        :rtype: str
        """
        ret = ""
        for astr in strs:
            ret += str(len(astr))
            ret +=":"
            ret +=astr 
        return ret

    def decode(self, s):
        """Decodes a single string to a list of strings.
        
        :type s: str
        :rtype: List[str]
        """
        ret=[]
        beg=0
        while beg<len(s):
            sepind=s.find(":", beg)
            if sepind<beg:
                break
            lenstr=s[beg:sepind]
            slen = int(lenstr)
            beg = sepind+1+slen
            onestr = s[sepind+1:beg]
            ret.append(onestr)
        return ret
        
if __name__ == "__main__":
    strs = [""]
    codec = Codec()
    encoderet = codec.encode(strs)
    print(encoderet)
    codec.decode(encoderet)
# Your Codec object will be instantiated and called as such:
# codec = Codec()
# codec.decode(codec.encode(strs))