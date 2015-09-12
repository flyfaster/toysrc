//============================================================================
// Name        : longestPalindrome.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
// Given a string S, find the longest palindromic substring in S. You may assume
// that the maximum length of S is 1000, and there exists one unique longest palindromic substring.
//============================================================================

#include <iostream>
#include <deque>
#include <stdarg.h>
#include <stdio.h>
using namespace std;
#define VERBOSE 1
class Solution {
public:
    void dbgOutput(const char* szFormat, ...)
	{
		char szBuff[1024];
#ifdef VERBOSE
        va_list arg;
		va_start(arg, szFormat);
		vsnprintf (szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);
		std::cout << szBuff;
#endif
	}
	size_t maxlen; size_t pos;
	size_t findPalindromeLen(string& s, size_t center) { // pattern "aba"
		dbgOutput("%s(%zd)\n", __FUNCTION__, center);
		if (center+maxlen/2>=s.length())
			return 0;
		size_t palindromelen = 1;
		if (s[center+maxlen/2] != s[center - maxlen/2] ) // check boundary first
			return 0;
		while(center+palindromelen/2<s.length() && center - palindromelen/2>=0) {
			if (s[center+palindromelen/2] == s[center - palindromelen/2] ) {
				palindromelen +=2;
			}
			else {
				palindromelen -= 2;
				break;
			}
		}
		if (center+palindromelen/2>=s.length() || center - palindromelen/2<0)
			palindromelen -= 2;
		if (palindromelen > maxlen) {
			maxlen = palindromelen;
			pos = center;
		}
		return maxlen;
	}
	size_t findPalindromeLen2(string& s, size_t center) { // pattern: "aabb"
		dbgOutput("%s(%zd)\n", __FUNCTION__, center);
		if (center+maxlen/2+1>=s.length())
			return 0;
		size_t palindromelen = 0;
		if (s[center+maxlen/2+1] != s[center - maxlen/2] )
			return 0;
		for(size_t i=0; i<maxlen/2; i++) {
			if(center+i+1>=s.length() || center-i<0)
				break;
			if(s[center+i+1]!=s[center-i])
				return 0;
		}

		palindromelen = maxlen -(maxlen%2);
		while(center+palindromelen/2+1<s.length() && center - palindromelen/2>=0) {
			if (s[center+palindromelen/2+1] == s[center - palindromelen/2] ) {
				palindromelen +=2;
				pos = center;
			}
			else
				break;
		}
		return maxlen=max(maxlen, palindromelen);
	}
    string longestPalindrome(string s) {
    	maxlen = 1; pos = 0;
    	if(s.length()<2) return s;
    	for(int i=0; i<s.length(); i++) {
    		findPalindromeLen(s, i);
    		findPalindromeLen2(s, i);
    	}
    	return s.substr((maxlen%2)?pos-maxlen/2:pos-maxlen/2+1, maxlen);
    }
};
int main() {
	cout << "https://leetcode.com/problems/longest-palindromic-substring/" << endl;
	Solution mysol;
	cout << mysol.longestPalindrome("ccc") <<endl;
	return 0;
}

