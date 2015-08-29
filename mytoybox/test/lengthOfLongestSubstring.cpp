#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <deque>
#include <cstring>
using namespace std;

class Solution {
public:
	typedef std::array<int,128> acnts_type;
	bool hasRepeat(const acnts_type& lastpos, const acnts_type& start, const acnts_type& end) {
		for(int i=0; i<start.size(); i++) {
			if (end[i]-lastpos[i]<=1 || end[i]==start[i])
				continue;
//			std::cout << "duplicate char found: " << (char)i<<std::endl;
			return true;
		}
		return false;
	}
    int lengthOfLongestSubstring(string s) {
    	// sliding window because max length is 26 or 52
    	// each position update char count  size_t cnt[26];
    	// just move forward.
    	// O(n) running time, O(1) space
    	int maxlen = s.length()?1:0;
    	std::deque<acnts_type> cntsarray;
    	acnts_type acnts, lastpos;
    	memset(acnts.data(), 0, acnts.size());
    	cntsarray.push_back(acnts);
    	for(auto onechar: s){
    		acnts[onechar] = acnts[onechar] + 1;
    		cntsarray.push_back(acnts);
    	}
    	int j = 0;
    	for(int i=1; i<=s.length(); i++) {
    		if(j<=i)
    			j=i+1;
    		for(; j<=s.length(); j++) {
    			if(hasRepeat(cntsarray[i-1], cntsarray[i], cntsarray[j])) {
//    				std::cout << "start "<< i <<", end "<<j<<std::endl;
    				maxlen = std::max(maxlen, j-i);
    				break;
    			}
//    			std::cout << "no duplicate start "<< i <<", end "<<j<<std::endl;
    			maxlen = std::max(maxlen, j-i+1);
    		}
    	}
    	return maxlen;
    }
};

int main() {
	cout << "https://leetcode.com/problems/longest-substring-without-repeating-characters/" << endl; // prints https://leetcode.com/problems/longest-substring-without-repeating-characters/
//	input = "dvdf";
	input = "au";
	Solution mysol;
	cout << mysol.lengthOfLongestSubstring(input) <<std::endl;
			return 0;
}
//Longest Substring Without Repeating Characters Total Accepted: 87741 Total Submissions: 434108
//
//Given a string, find the length of the longest substring without repeating characters. For example,
//the longest substring without repeating letters for "abcabcbb" is "abc", which the length is 3.
//For "bbbbb" the longest substring is "b", with the length of 1.
