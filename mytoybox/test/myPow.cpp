//============================================================================
// Name        : myPow.cpp
//============================================================================

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
using namespace std;
class Solution {
public:
    void dbgOutput(const char* szFormat, ...)
	{
		char szBuff[1024];
		va_list arg;
		va_start(arg, szFormat);
		vsnprintf (szBuff, sizeof(szBuff), szFormat, arg);
		va_end(arg);
//		std::cout << szBuff;
	}
    template <typename T>
    T myPowT(T x, int n) {
    	std::unordered_map<int, T> powertable;
    	//std::cout << __func__<<"("<<x<<","<<n<<") ";
    	if(n<0) {
    		return (T)1/(x*myPowT(x, -1-n));
    	}
        if(n==0) return T(1);
        if(n==1) return x;
        if(n==2) return x*x;
        int i = 1; int exponent = n;
        if (powertable.size()<1) {
            powertable[1]=x; // powertable[2]=x*x;
            do {
                if (i*2<0) break;
                i = i*2;
                powertable[i] = powertable[i/2]*powertable[i/2];
               // dbgOutput("pow(%L,%d)=%L\n", x, i,powertable[i]);
//                std::cout <<"pow(" << x<<","<<i<<")="<<powertable[i]<<std::endl;

            } while(i<n);
        }
        T ret = 1;
        while(n>0){
            while(i>n) i = i/2;
            if(n>=i) {
                ret = ret * powertable[i];
                n -= i;
            }
        }  //dbgOutput("%s(%f,%d) return %f\n", __FUNCTION__, x, n, ret);
//        std::cout << __FUNCTION__<<"(" << x<<","<<exponent<<")="<<ret<<std::endl;
        return ret;
    }
    double myPow(double x, int n) {
            return myPowT(x,n);
        }
};
int main() {
	cout << "powx-n/" << endl; // 
	Solution mysol;
//	cout << mysol.myPow(0.00001, 2147483647) << std::endl;
	cout << mysol.myPow(8.84372, -5) << std::endl;
//	cout << mysol.myPow(8.84372, 3) << std::endl;
//	cout << mysol.myPow(8.84372, 4) << std::endl;
	cout << mysol.myPow(8.84372, 5) << std::endl;
	cout << 1.0/mysol.myPow(8.84372, 5) << std::endl;
	cout << mysol.myPowT(3, 3) << std::endl;
	cout << mysol.myPowT(1.00000, 2147483647) << std::endl;
	cout <<pow(4,-2) <<std::endl;
	cout <<pow(1.00000, 2147483647) <<std::endl;
	return 0;
}

