#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    bool isgroup(const string& lhs, const string& rhs)
    {
        int diff = 0;
        for(int i=0; i<lhs.length(); i++)
        {
            if(lhs[i]!=rhs[i])
            {
                ++diff;
                if(diff>2)
                    return false;
            }
        }
        return diff==2;
    }
    unordered_map<string, unordered_set<string>> group;
    bool mergegroup(const string& lhs, const string& rhs)
    {
        unordered_map<string, unordered_set<string>>::iterator iter = group.find(rhs);
        if (iter==group.end() || iter->second.empty())
            return false;
        if (lhs==rhs)
            return false;
        unordered_map<string, unordered_set<string>>::iterator liter = group.find(lhs);
        if (liter==group.end() || liter->second.empty())
        	return false;
        copy(iter->second.begin(), iter->second.end(), inserter(group[lhs], group[lhs].begin()));
        group[rhs].clear();
        return true;
    }
    void output(const string& str)
    {
    	std::cout << str << " ";
    }
    int numSimilarGroups(vector<string>& A) {
        int res = 0;
        unordered_map<string,string> ancestors;
        for(int i=0; i<A.size(); ++i)
        {
        	auto lhs_anc_iter = ancestors.find(A[i]);

            for(int j=i+1; j<A.size(); ++j)
            {
                if(isgroup(A[i], A[j]))
                {
                	if (lhs_anc_iter!=ancestors.end()) {
                		group[lhs_anc_iter->second].insert(A[j]);
                		ancestors[A[j]]=lhs_anc_iter->second;
                	}
                	else {
                		auto rhs_anc_iter = ancestors.find(A[j]);
                		if (rhs_anc_iter!=ancestors.end()) {
							group[rhs_anc_iter->second].insert(A[i]);
							ancestors[A[i]]=rhs_anc_iter->second;
						}
                    	else {
                    		group[A[i]].insert(A[j]);
                    		ancestors[A[j]] = A[i];
                    	}
                	}

                }
            }
            if (group.find(A[i])==group.end() && ancestors.find(A[i]) == ancestors.end())
            {
                ++res;
                //output(A[i] + "\n");
            }
        }

        // merge groups 
        int merge_cnt = 0;
        
        while(group.size()>1) {
            merge_cnt =0;
            for(auto iter: group)
            {
                auto& vec = iter.second;
                for(auto wd: vec)
                {
                    if (mergegroup(iter.first, wd))
                        merge_cnt++;
                }
            }
            for(auto iter=group.begin(); iter!=group.end();)
            {
                if(iter->second.empty())    iter = group.erase(iter);
                else    ++iter;
            }
            if(merge_cnt==0 )
                break;
        }
        return res + group.size();
    }
};

int main()
{
	vector<string> A{"tars","rats","arts","star"};
	A = {"nmiwx","mniwx","wminx","mnixw","xnmwi"};
	std::cout << Solution().numSimilarGroups(A) << std::endl;
	return 0;
}
