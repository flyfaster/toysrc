class Solution { // single-number-iii
public:
    vector<int> singleNumber(vector<int>& nums) {
        int xd = 0; vector<int> ret;
        for(auto data: nums) xd = xd ^ data;
        int a=0; int b=0;
        int mask = (xd & (xd ^ (xd-1)));
        for(auto data: nums) {
            if(mask & data)
                a = a ^ data;
            else
                b = b ^ data;
        }
        ret.push_back(a^xd);
        ret.push_back(b^xd);
        return ret;
    }
};
