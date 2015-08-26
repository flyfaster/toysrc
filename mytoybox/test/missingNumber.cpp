class Solution {
public:
    int missingNumber(vector<int>& nums) {
        int nmin = nums[0]; int nmax = nmin;
        int64_t sum  = 0;
        for(auto data: nums) {
        	nmin = std::min(data, nmin);
        	nmax = std::max(data, nmax);
        	sum += data;
        }
        int64_t expectedsum = (nmin+nmax)*(nmax-nmin+1)/2;
        int missing = expectedsum-sum;
        if (missing ==0) {
        	if(nmin>0)
        		return nmin-1;
        	else
        		return nmax+1;
        }
        return missing;
    }
};
