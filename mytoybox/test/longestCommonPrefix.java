public class Solution {
	// Submission Result: Accepted
    public String longestCommonPrefix(String[] strs) {
        int cp=-1;
        boolean all=true;
        while(all && strs.length>0) {
            for(int i=0; i<strs.length; i++) {
                if(strs[i].length()<=cp+1)
                {
                    all=false;
                    break;
                }
                if(strs[0].charAt(cp+1) != strs[i].charAt(cp+1))
                {
                    all=false;
                    break;
                }
            }
            if(all) cp++;
        }
        if(cp+1>0)
        return strs[0].substring(0, cp+1);
        return "";
    }
}