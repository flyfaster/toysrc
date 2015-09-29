// https://leetcode.com/problems/h-index-ii/
class Solution {
public:
    int hIndex(vector<int>& citations) {
        int ret = 0;
        int size = citations.size();
        for(int i=0; i<size; i++) {
            int index = size-i-1;
            if(citations[index]>=i+1)
                ret = i+1;
        }
        return ret;
    }
};

// https://leetcode.com/problems/h-index/
class Solution {
public:
    int hIndex(vector<int>& citations) {
        std::sort(citations.begin(), citations.end(), std::greater<int>());
        int ret=0;
        for(int i=0; i<citations.size(); i++) {
            if(citations[i]>=i+1)
                ret = i;
        }
        if(citations.size() && citations[0])
        return ret+1;
        return ret;
    }
};
