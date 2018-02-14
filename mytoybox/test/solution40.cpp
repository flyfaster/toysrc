class Solution {
public:
    using row_type = map<int,int>;
    static void sum(set<row_type>& res, vector<int>& candidates, int target, int pos, vector<int>& param) {
        if (target==0) {
            row_type row;
            for (int i=0; i<pos; ++i) {
                if (param[i])
                    row[candidates[i]]++;
            }
            res.insert(row);
        }
        if (target<=0||pos>=candidates.size())
            return;
        // if (param.size()<=pos)
        //     param.push_back(0);
        param[pos]=0;
        sum(res, candidates, target, pos+1, param);
        param[pos]=1;
        sum(res, candidates, target-candidates[pos], pos+1, param);
    }
    // 30ms beat 6.15%
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        std::sort(candidates.begin(), candidates.end());
        set<row_type> tmp;
        
        vector<int> param(candidates.size());
        sum(tmp, candidates, target, 0, param);
        vector<vector<int>> res;
        for (const auto& dd: tmp) 
        {
            vector<int> row;
            for (const auto& iter: dd) {
                for (int i=0; i<iter.second; ++i)
                    row.push_back(iter.first);
            }
            res.emplace_back(row);
        }
        return res;
    }
};