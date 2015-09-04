//============================================================================
// Name        : numberToWords.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <sstream>
using namespace std;
class Solution {
public:
    string singleDigit(int num) {
        std::stringstream ss;
        switch(num){
            case 0: break;
            case 1: ss<<"One "; break;
            case 2: ss<<"Two "; break;
            case 3: ss<<"Three "; break;
            case 4: ss<<"Four "; break;
            case 5: ss<<"Five "; break;
            case 6: ss<<"Six "; break;
            case 7: ss<<"Seven "; break;
            case 8: ss<<"Eight "; break;
            case 9: ss<<"Nine "; break;
            case 10: ss<<"Ten "; break;
            case 11: ss<<"Eleven "; break;
            case 12: ss<<"Twelve "; break;
            case 13: ss<<"Thirteen "; break;
            case 14: ss<<"Fourteen "; break;
            case 15: ss<<"Fifteen "; break;
            case 16: ss<<"Sixteen "; break;
            case 17: ss<<"Seventeen "; break;
            case 18: ss<<"Eighteen "; break;
            case 19: ss<<"Nineteen "; break;
        }
        return ss.str();
    }
   string numberToStr(int num) {
        std::stringstream ss;
        if(num>=100)
            ss << singleDigit(num/100) << "Hundred ";
        num = num%100;
        if(num>=20)
        {int temp = num-(num%10);
            switch(temp){
            case 20: ss<<"Twenty "; break;
            case 30: ss<<"Thirty "; break;
            case 40: ss<<"Forty "; break;
            case 50: ss<<"Fifty "; break;
            case 60: ss<<"Sixty "; break;
            case 70: ss<<"Seventy "; break;
            case 80: ss<<"Eighty "; break;
            case 90: ss<<"Ninety "; break;
            default: break;
            }
            num = num%10;
        }
        ss << singleDigit(num);
        return ss.str();
    }
    string numberToWords1(int num) {
        if (num==0) return "Zero";
        if(num<1000) return numberToStr(num);
        std::stringstream ss;
        if(num<1000*1000) {
            int high = num / 1000;
            ss<< numberToStr(high) << "Thousand ";
            if (num%1000)
            ss << numberToStr(num%1000);
            return ss.str();
        }
        if (num <1000*1000*1000) {
        int nm = num/(1000*1000);
        ss << numberToStr(nm) << "Million ";
        num = (num%(1000*1000));
        if(num)
            ss << numberToWords(num);
        return ss.str();
        }
        int bnum = num / (1000*1000*1000);
        ss << singleDigit(num / (1000*1000*1000)) <<"Billion ";
        if(num % (1000*1000*1000)) ss<<numberToWords(num % (1000*1000*1000));
        return ss.str();
    }
    string numberToWords(int num) {
        std::string ret = numberToWords1(num);
        while(ret.back() == ' ')
        {
            ret = ret.substr(0, ret.length()-1);
        }
        return ret;
    }
};
int main() {
	cout << "https://leetcode.com/problems/integer-to-english-words/" << endl; // prints https://leetcode.com/problems/integer-to-english-words/
	Solution mysol;
	cout << "\"" << mysol.numberToWords(123)<<"\"" << endl;
	cout << "\"" << mysol.numberToWords(1234567)<<"\"" << endl;
	cout << "\"" << mysol.numberToWords(1234567891)<<"\"" << endl;
	return 0;
}

