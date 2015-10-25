//============================================================================
// Name        : evalRPN.cpp
//============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
using namespace std;

class Solution {
public:
	bool is_num(const string& tok) {
		return isdigit(tok[0]) || tok.length()>1;
	}
    int evalRPN(vector<string>& tokens) {
        stack<string> rpnstack;
        for(const auto tok: tokens) {
        	if (is_num(tok)) {
        		rpnstack.push(tok);
        		continue;
        	}
        	int d1, d2, ret;
        	 d1 = atoi(rpnstack.top().c_str());
        	rpnstack.pop();
        	 d2 = atoi(rpnstack.top().c_str());
        	rpnstack.pop();
        	 if (tok == "+" ) {
        		 ret = d1+d2;
        	 }
        	 if (tok=="-") {
        		 ret = d2-d1;
        	 }
        	 if (tok=="*") {
        		 ret = d2*d1;
        	 }
        	 if (tok=="/")
        		 ret = d2/d1;
        	 char buf[16]; sprintf(buf, "%d", ret);
        	 rpnstack.push(buf);
        }
        return atoi(rpnstack.top().c_str());
    }
};

int main() {
	cout << "evaluate-reverse-polish-notation/" << endl;
	return 0;
}

