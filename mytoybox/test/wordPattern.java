import java.util.HashMap;
import java.util.Map;
import java.util.*;

public class Solution {
    public boolean wordPattern(String pattern, String str) {
    	HashMap<Character, String> patternmap = new HashMap<Character, String>();
    	HashMap<String, Character> rmap = new HashMap<String, Character>();
    	int pos = 0; int spacepos=0;
    	for(int i=0; i<pattern.length(); i++) {
    		if(pos>=str.length())
    			return false;
    		spacepos = str.indexOf(' ', pos);
    		if (spacepos<0)
    			spacepos = str.length();
    		String word = str.substring(pos, spacepos);
    		pos = spacepos+1;
    		Character ch = pattern.charAt(i);
    		if(patternmap.containsKey(ch)){
    			if(patternmap.get(ch).compareTo(word)!=0){
    				//System.out.format("'%s' not match '%s'\n", word, patternmap.get(ch));
    				return false;
    			}
    		} else {
    			if (rmap.containsKey(word))
    				return false;
    			rmap.put(word, ch);
    			patternmap.put(ch, word);
    		}
    	}
    	if(pos<str.length())
    		return false;
    	return true;
    }
    public static void main(String[] argv) {
    	Solution mysol = new Solution();
    	if(mysol.wordPattern("abba", "dog cat cat dog"))
    		System.out.println("true");
    	else
    		System.out.println("false");
    }
}
//C:\onega\leetcode>"C:\Program Files\Java\jdk1.8.0_60\bin\javac.exe" Solution.java
//C:\onega\leetcode>java -cp . Solution

