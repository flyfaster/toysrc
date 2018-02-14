class Solution {
public:
    static void sum39(const vector<int>& candidates, int target, int pos, vector<vector<int>>& res, vector<int>& param)
    {
        if (pos>=candidates.size())
            return;
        
        for (int i=0; i<=target/candidates[pos]; ++i)
        {
            if (param.size()>pos)
                param[pos] = i;
            else
                param.push_back(i);
            
            if (target == candidates[pos]*i)
            {
                vector<int> row;
                for (int j=0; j<=pos; ++j)
                {
                    for (int k=1; k<=param[j]; ++k)
                        row.push_back(candidates[j]);
                }
                res.emplace_back(row);
                continue;
            }
            sum(candidates, target-candidates[pos]*i, pos+1, res, param);
        }
    }
    static void sum(const vector<int>& candidates, int target, int pos, vector<vector<int>>& res, vector<int>& param)
    {
        if (target == 0)
        {
            vector<int> row;
            for (int j=0; j<pos; ++j)
            {
                for (int k=1; k<=param[j]; ++k)
                    row.push_back(candidates[j]);
            }
            res.emplace_back(row);
            return;
        }
        
        if (pos>=candidates.size())
            return;

        for (int i=0; i<=target/candidates[pos]; ++i)
        {
            if (param.size()>pos)
                param[pos] = i;
            else
                param.push_back(i);
            

            sum(candidates, target-candidates[pos]*i, pos+1, res, param);
        }
    } // 15ms beat 41%
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> res;
        vector<int> param;
        sum(candidates, target, 0, res, param);
        return res;
    }
};