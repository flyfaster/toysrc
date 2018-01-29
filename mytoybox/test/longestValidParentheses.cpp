#include <string>
#include <utility>
#include <deque>
#include <vector>
using namespace std;

class Solution {
public:
    int longestValidParentheses(string s) { // 13ms beat 13.8%
        bool stop = false;
        do {
            stop = true;
            size_t lpos = 0;
            while (lpos!=string::npos) {
                lpos = s.find(')', lpos);
                if (lpos==string::npos )
                    break;
                auto rpos = s.rfind('(', lpos);
                if (rpos!=string::npos) {
                    auto lpos2 = s.find_first_of(')', rpos );
                    if (lpos2 == lpos) {
                        s[lpos] = 0;
                        s[rpos] = 0;
                        stop = false;
                    }
                }
                ++lpos;
            }            
        } while(!stop);
        int cnt = 0;
        int ret = 0;
        for (auto c: s)
        {
            if (c==0) {
                ++cnt;
                ret = std::max(ret, cnt);
            } else
                cnt = 0;
        }
        return ret;
    }
    int longestValidParentheses4(string s) { // 39ms
        bool stop = false;
        do {
            stop = true;
            size_t lpos = 0;
            while (lpos!=string::npos) {
                lpos = s.find(')', lpos);
                if (lpos!=string::npos ) {
                    auto rpos = s.rfind('(', lpos);
                    auto lpos2 = (lpos>0)?s.rfind(')', lpos-1):string::npos;
                    if (rpos!=string::npos) {
                        if (lpos2!=string::npos){
                            if (rpos>lpos2) {
                                s[lpos] = 0;
                                s[rpos] = 0;
                                stop = false;
                            }
                        }
                        else {
                            s[lpos] = 0;
                            s[rpos] = 0;
                            stop = false;
                        }
                    }
                    ++lpos;
                }
            }            
        } while(!stop);
        int cnt = 0;
        int ret = 0;
        for (auto c: s)
        {
            if (c==0) {
                ++cnt;
                ret = std::max(ret, cnt);
            } else
                cnt = 0;
        }
        return ret;
    }    
    int longestValidParentheses3(string s) { // 36ms
        int ret = 0;
        bool stop = true;
        do {
            stop = true;
            for (int i=0; i<s.length(); i++) {
                if (s[i]==')' && i>0)
                {
                    auto pos = s.rfind('(', i);
                    auto pos2 = s.rfind(')', i-1);
                    if (pos!=string::npos)
                    {
                        if (pos2!=string::npos)
                        {
                            if (pos2<pos)
                            {
                                s[i] = 0;
                                s[pos] = 0;
                                stop = false;
                            }
                        }
                        else
                        {
                            s[i] = 0;
                            s[pos] = 0;
                            stop = false;
                        }
                    }
                }
            }
        } while(!stop);
        int cnt = 0;
        for (auto data: s)
        {
            if (data==0) {
                ++cnt;
                ret = std::max(ret, cnt/2);
            }
            else
                cnt = 0;
        }
        return ret;
    }
    int longestValidParentheses2(string s) { // 30ms beat 5%
        int ret = 0;
        using coltype=deque<int>;
        coltype col;
        vector<int> marks(s.length());
        
        for (int i=0; i<s.length(); i++)
            col.emplace_back( i);

        bool haspair = false;
        do {
            haspair = false;
            int i=col.size()-1;
            for (; i>0; )
            {
                if (s[col[i]]==')' && s[col[i-1]]=='(')
                {
                    marks[col[i]] = 1;
                    marks[col[i-1]] = 1;
                    col.erase(col.begin()+i-1, col.begin()+i+1);
                    --i;
                    haspair = true;
                }
                --i;
            }
        } while (haspair);
        // find consecutive 1 in marks
        int cnt = 0;
        for (auto data: marks)
        {
            if (data)
            {
                cnt++;
                ret = std::max(ret, cnt);
            }
            else
                cnt = 0;
        }
        return ret;
    }
    int longestValidParentheses1(string s) {    // 54ms beat 4.66%
        int ret = 0;
        using nodetype=pair<char,int>;
        using coltype=deque<nodetype>;
        coltype col;
        vector<int> marks(s.length());
        
        for (int i=0; i<s.length(); i++)
            col.emplace_back(s[i], i);

        bool haspair = false;
        do {
            haspair = false;
            int i=col.size()-1;
            for (; i>0; )
            {
                if (col[i].first==')' && col[i-1].first=='(')
                {
                    marks[col[i].second] = 1;
                    marks[col[i-1].second] = 1;
                    col.erase(col.begin()+i-1, col.begin()+i+1);
                    --i;
                    haspair = true;
                }
                --i;
            }
        } while (haspair && col.empty()==false);
        // find consecutive 1 in marks
        int cnt = 0;
        for (auto data: marks)
        {
            if (data)
            {
                cnt++;
                ret = std::max(ret, cnt);
            }
            else
                cnt = 0;
        }
        return ret;
    }
};

int main()
{
    string input = "()(()";
    return 0;
}