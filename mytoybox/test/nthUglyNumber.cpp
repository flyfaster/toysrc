class Solution {
public:
    bool isUgly(int num) {
        if (num == 1) return true;
        while( (num%2) == 0 && num >=2)
          num = num/2;
        while( (num%3) == 0 && num >=3)
          num = num/3; 
        while( (num%5) == 0 && num >=5)
          num = num/5;     
        return num==1;
    }
    int increase(std::set<int>& src, int last) {
    	std::set<int>dst = src;
    	for(auto ss: src) {
    		if(ss > last/2 && ss <= std::numeric_limits<int>::max()/2)
    		dst.insert(ss *2);
    		if(ss > last/3 && ss <= std::numeric_limits<int>::max()/3)
    		dst.insert(ss *3);
    		if(ss > last/4 && ss <= std::numeric_limits<int>::max()/4)
    		dst.insert(ss *4);
    		if (ss > last/5 && ss <= std::numeric_limits<int>::max()/5)
    		dst.insert(ss *5);
    	}
    	src.clear();
    	swap(src, dst);
    	return last*3;
    }
    int nthUglyNumber(int n) {
    	std::set<int> uglyset;
    	for(int i=1; i<30; i++) {
    		if(isUgly(i)) {
    			uglyset.insert(i);
    		}
    	}
    	int last = *uglyset.rbegin();
    	int maxsize = n+n/2+50;
    	while(uglyset.size()<maxsize ) {
    		int before = uglyset.size();
    		last = increase(uglyset, last);
    		if(before == uglyset.size())
    			break;
    	}
    	//std::cout <<__func__ <<" size " << uglyset.size()<<std::endl;
    	int ret = 0; int cnt=0;
    	for(auto aa: uglyset) {
    		cnt++;
    		if(cnt==n) {
    			ret = aa;
    			break;
    		}
    	}
        return ret;
    }
};
