// Forward declaration of isBadVersion API.
bool isBadVersion(int version);

class Solution {
public:
    int firstBadVersion(int n) {
        int lastgood = 0;
        int start=0, end = n;
        while(true) {
            int mid = start/2+end/2;
            if(start%2 && end%2)  mid++;
            if(isBadVersion(mid)) {
                if(mid-lastgood<=1)
                    return mid;
                end = mid;
                continue;
            }
            if(start<mid)
                start = mid;
            else start = mid+1;
            lastgood = mid;
        }
        return lastgood+1;
    }
};
