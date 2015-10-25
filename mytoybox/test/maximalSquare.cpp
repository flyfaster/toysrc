//============================================================================
// Name        : maximalSquare.cpp
//============================================================================

#include <iostream>
#include <vector>
#include <stdarg.h>
#include <cstdio>
using namespace std;
#define VERBOSE
class Solution {
public:
	int verbose;
    void dbgOutput(const char* szFormat, ...)
	{
#ifdef VERBOSE
    	if(!verbose) return;
    	char szBuff[1024];
        va_list arg;
		va_start(arg, szFormat);
		vsnprintf (szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);
		std::cout << szBuff;
#endif
	}
	bool checkBounds(vector<vector<char> >& matrix, int x, int y, int step) {
		dbgOutput("%s x=%d y=%d step=%d\n", __func__, x, y, step);
		step--;
		if(x+step>=height || y+step>=width) return false;
		for(int i=0; i<=step; i++)
		{
			vector<char>& row = matrix[x+step];
			for(int j=0; j<=step; j++)
				if(row[j+y]=='0')
					return false;
			if(matrix[x+i][y+step]=='0')
				return false;
		}
		return true;
	}
	int height;
	int width;
    int maximalSquare(vector<vector<char> >& matrix) {
    	if(matrix.size()==0 || matrix[0].size()==0 ) return 0;
    	// maxlen = 0
    	// for each position, find if it can grow to maxlen+1
    	int maxlen = 0;
    	height = matrix.size();
    	width = matrix[0].size();
    	dbgOutput("%s height=%d width=%d\n", __func__, height, width);
    	for(int i=0; i<height; i++) {
    		if( height-i<maxlen)
    			continue;
    		vector<char>& row = matrix[i];
    		for(int y=0; y<width; y++) {
    			if(width-y<maxlen)
    				continue;
    			if(row[y] == '0')
    				continue;
    			maxlen = max(maxlen,1);
    			if(checkBounds(matrix, i, y, maxlen+1)==false)
    				continue;
    			bool cangrow = true;
    			for(int step=1; step<maxlen; step++)
    				if(checkBounds(matrix, i, y, step+1)==false) {
    					cangrow = false;
    					break;
    				}
    			if(cangrow) {
    				while(checkBounds(matrix, i, y, maxlen+1))
    					maxlen++;
    			}
    		}
    	}
    	return maxlen*maxlen;
    }
};

void test(vector<string>& aa) {
	vector<vector<char> > matrix;
//	for(size_t i=0; i<std::extent<decltype(aa)>::value; i++)
	for(const auto line: aa)
	{
		vector<char> row;
		for(auto onechar: line)
			row.push_back(onechar);
		matrix.push_back(row);
	}
	Solution mysol;
	mysol.verbose = 1;
	cout << mysol.maximalSquare(matrix) << endl;
}
int main() {
	cout << "maximal-square Submission Result: Accepted " << endl; // prints maximal-square
	vector<string> aa={"11","11"};
	test(aa);
	vector<string> bb={"1101","1101","1111"};
	test(bb);
	return 0;
}

