#include <limits>
class Solution {
public:
    int divide(int dividend, int divisor) {
        if (divisor==0)
            return std::numeric_limits<int>::max();
        bool signchange=false;
        if (dividend>0 && divisor<0)
            signchange=true;
        if (dividend<0 && divisor>0)
            signchange = true;
        if (dividend<0) dividend = 0-dividend;
        if (divisor<0) divisor = 0-divisor;
        if (divisor==1) return dividend;
        if (divisor==-1) return 0-dividend;
        
        if (dividend<divisor)
		    return 0;
	int times = 1;
	int j = 1;
	while(times*divisor>dividend || times*divisor<=dividend-divisor) {
		j = 1;
		while(times*divisor>dividend) {
			times = times-j;
			j = j+j;
		}
		j = 1;
		while(times*divisor<=dividend-divisor) {
			times += j;
			j = j+j;
		}
	}
	
	if (signchange) times = 0-times;
	
	return times;
    }
};
