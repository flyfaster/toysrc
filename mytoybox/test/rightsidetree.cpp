/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    vector<int> rightSideView(TreeNode *root) {
        vector<int> ret;
        if (root == nullptr)
            return ret;
            
        std::queue<TreeNode*> bfsq; // Breadth first traversal
        std::map<TreeNode*, int> nodelevel;
        bfsq.push(root);
        nodelevel[root]=0;
        int current_level=0;
        TreeNode* lastnode = root;
        TreeNode* current = nullptr;
        while(bfsq.size()) {
            current = bfsq.front();
            bfsq.pop();
           
            int level = nodelevel[current];
            if (current->left) {
                bfsq.push(current->left);
                nodelevel[current->left] = level+1;
            }
            if (current->right) {
                bfsq.push(current->right);
                nodelevel[current->right] = level+1;
            }
            if (level > current_level) { // first node of next level
                ret.push_back(lastnode->val);
                current_level = level;
            }
            lastnode = current;
            
        }
        if (lastnode)
             ret.push_back(lastnode->val);
        return ret;
    }
};
