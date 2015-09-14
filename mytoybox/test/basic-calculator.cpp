//============================================================================
// Name        : basic-calculator.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
//Implement a basic calculator to evaluate a simple expression string.
//The expression string may contain open ( and closing parentheses ), the plus + or minus sign -,
//non-negative integers and empty spaces .
//You may assume that the given expression is always valid.
//Some examples:
//"1 + 1" = 2
//" 2-1 + 2 " = 3
//"(1+(4+5+2)-3)+(6+8)" = 23
//Note: Do not use the eval built-in library function.
//============================================================================

#include <iostream>
#include <string>
#include <deque>
#include <stack>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include <stdio.h>
#include <stdarg.h>
#include <locale>
#define VERBOSE 1
using namespace std;

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
	string addspace(string& s) {
		// add space for easy parse
		std::stringstream ss;
		for(const auto onechar: s) {
			switch(onechar) {
			case '(':
			case ')':
			case '+':
			case '-':
			case '*':
			case '/':
				ss << ' ' << onechar << ' ';
				break;
			default:
				ss << onechar;
				break;
			}
		}
		return ss.str();
	}
	void parsetokens(string& s, deque<string>& tokens) {
		char* buf = new char[s.length()+1];
		strcpy(buf, s.c_str());
		char* pch = strtok(buf, " ");
		while(pch) {
			tokens.push_back(pch);
			pch = strtok(nullptr, " ");
		}
		delete[] buf;
	}
	bool is_operator(const string& op) {
		return op == "+" || op=="-";
	}
	bool is_num(const string& tok) {
		return isdigit(tok[0]) || tok.length()>1;
	}
	void toRPN(deque<string>& tokens, deque<string>& rpn) {
		stack<string> ops;
		for(const auto tok: tokens) {
			if( is_num(tok) ) {
				rpn.push_back(tok);
				continue;
			}
			if(is_operator(tok)) {
				while(ops.empty()==false && is_operator(ops.top())) {
					rpn.push_back(ops.top());
					ops.pop();
				}
				ops.push(tok);
				continue;
			}
			if(tok == "(") {
				ops.push(tok);
				continue;
			}
			if (tok == ")") {
				while (ops.empty() == false && ops.top()!="(") {
					rpn.push_back(ops.top());
					ops.pop();
				}
				ops.pop();
			}
		}
		while (!ops.empty()) {
			rpn.push_back(ops.top()); ops.pop();
		}
	}
	int evaluateRPN(deque<string>& rpn) {
		int ret = 0; stack<string> tmp;
		for(const auto tk: rpn) {
			if (is_num(tk)) {
				tmp.push(tk);
				continue;
			}
			if (is_operator(tk)) {
				int d1 = atoi(tmp.top().c_str());
				tmp.pop();
				int d2 = atoi(tmp.top().c_str());
				tmp.pop();
				ret = d1+d2;
				if(tk=="-")
					ret = d2-d1;
				char buf[16]; sprintf(buf, "%d", ret);
				tmp.push(buf);
				dbgOutput("%s %d %s %d = %d\n", __FUNCTION__, d1, tk.c_str(), d2, ret);
			}
		}
		ret = atoi(tmp.top().c_str()); dbgOutput("%s  top is %s\n", __FUNCTION__, tmp.top().c_str());
		return ret;
	}
    int calculate(string s) {
    	string transformedstr = addspace(s);
    	deque<string> tokens;
    	parsetokens(transformedstr, tokens);
    	stack<string> ops, data;
    	int ret = 0;
    	deque<string> rpn;
    	toRPN(tokens, rpn);
    	ret = evaluateRPN(rpn);
    	return ret;
    }
};

int main() {
	cout << "https://leetcode.com/problems/basic-calculator/" << endl;
	Solution mysol;
	cout << mysol.calculate("(7)-(0)+(4)")<<endl;
	cout << mysol.calculate("2-(5-6)")<<endl;
	cout << mysol.calculate("1+(2+3)")<<endl;
	cout << mysol.calculate("1-11")<<endl;
	cout << mysol.calculate("4-5+2")<<endl;
	return 0;
}

