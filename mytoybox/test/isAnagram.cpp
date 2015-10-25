class Solution {
public:
    bool isAnagram(string s, string t) {
        std::map<char,size_t> scnts, tcnts;
        for(auto onechar: s) {
            if(scnts.find(onechar)!=scnts.end())
                scnts[onechar] = scnts[onechar]+1;
            else
                scnts[onechar] = 1;
        }
        for(auto onechar: t) {
            if(tcnts.find(onechar)!=tcnts.end())
                tcnts[onechar] = tcnts[onechar]+1;
            else
                tcnts[onechar] = 1;
        }
        if(scnts.size()!=tcnts.size()) return false;
        for(auto data:scnts) {
            if(tcnts.find(data.first)==tcnts.end())
                return false;
            if(tcnts[data.first]!=data.second)
                return false;
        }
        return true;
    }
};//valid-anagram
