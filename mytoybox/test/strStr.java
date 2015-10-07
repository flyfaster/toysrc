public class Solution {
    public int strStr(String haystack, String needle) {
        int srclen = haystack.length();
        int keylen = needle.length();
        if(srclen<keylen)
            return -1;
        int bound = srclen-keylen;
        int ret = 0;
        boolean allequal = true;
        for(int i=0; i<=bound; i++) {
            allequal = true;
            for(int j=0; j<keylen; j++) {
                if(haystack.charAt(i+j)!=needle.charAt(j)) {
                    allequal = false;
                    break;
                }
            }
            if(allequal)
                return i;
        }
        if(allequal) // when needle is null
            return ret;
        return -1;
    }
}
