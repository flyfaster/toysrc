//============================================================================
// Name        : count-complete-tree-nodes.cpp
//============================================================================
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <list>
#include <stack>
#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include <math.h>
using namespace std;

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
struct TreeNode
{
    int val=0;
    TreeNode* left=nullptr;
    TreeNode* right=nullptr;
    TreeNode(int x) : val(x)
    {
    }
};

class Solution
{
public:
    int leftHeight(TreeNode* root)
    {
        int height = 0;
        while (root)
        {
            height++;
            root = root->left;
        }
        return height;
    }

    int rightHeight(TreeNode* root)
    {
        int height = 0;
        while (root)
        {
            height++;
            root = root->right;
        }
        return height;
    }

    int countNodes(TreeNode* root)
    {
        if (root == nullptr)
            return 0;
        if (root->left == nullptr && root->right == nullptr)
            return 1;
        int leftheight = leftHeight(root);
        int rightheight = rightHeight(root);
        if (leftheight == rightheight)
            return pow(2, leftheight) - 1;
        return 1 + countNodes(root->left) + countNodes(root->right);
    }
};

class bst_max_path_sum
{
    std::unordered_map<TreeNode*, int> cache;
    int maxToRoot(TreeNode* root, int& re)
    {
        if (!root)
            return 0;
        int l = maxToRoot(root->left, re);
        int r = maxToRoot(root->right, re);
        if (l < 0)
            l = 0;
        if (r < 0)
            r = 0;
        if (l + r + root->val > re)
            re = l + r + root->val;
        // cache[root] = root->val + max(l, r);
        return root->val + max(l, r);
    }

public:
    int maxPathSum(TreeNode* root)
    {
        int max = std::numeric_limits<int>::min();
        maxToRoot(root, max);
        return max;
    }
};

class sum_root_to_leaf {
    int sum = 0;
    void sumtree(TreeNode* root, int parsum) {
        if (!root)
            return;
        parsum *= 10;
        parsum += root->val;
        if (!root->left && !root->right)
        {
            sum += parsum;
            return;
        }
        sumtree(root->left, parsum);
        sumtree(root->right, parsum);
    }
public:
    int sumNumbers(TreeNode* root) {
        sumtree(root, 0);
        return sum;
    }
};

class path_sum2 {
    vector<vector<int>> res;
    void leafsum(TreeNode* root, int target, int parsum, vector<int>& path)
    {
        if (!root)
            return;
        parsum += root->val;
        path.push_back(root->val);
        if (!root->left && !root->right)
        {
            // leaf
            if (parsum == target)
                res.push_back(path);
        }
        leafsum(root->left, target, parsum, path);
        leafsum(root->right, target, parsum, path);
        path.pop_back();
    }
public:
    vector<vector<int>> pathSum(TreeNode* root, int sum) {
        vector<int> path;
        leafsum(root, sum, 0, path);
        return res;
    }
};

class TraversalSolution {
    void inorder(TreeNode* root, vector<int>& res)
    {
        if (!root)
            return;
        inorder(root->left, res);
        res.push_back(root->val);
        inorder(root->right, res);
    }
    vector<int> inorderTraversalRecursive(TreeNode* root) {
        vector<int> res;
        inorder(root, res);
        return res;
    }
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> res;
        stack<TreeNode*> st;
        auto cur = root;
        while (cur || !st.empty())
        {
            if (cur)
            {
                st.push(cur);
                cur = cur->left;
            } else {
                cur = st.top();
                st.pop();
                res.push_back(cur->val);
                cur = cur->right;
            }
        }

        return res;
    }

    vector<int> preorderTraversal(TreeNode* root) {
        vector<int> res;
        stack<TreeNode*> s;
        s.push(root);
        while (!s.empty()) {
            TreeNode* p = s.top();
            s.pop();
            if (p) {
                res.push_back(p->val);
                s.push(p->right);
                s.push(p->left);
            }
        }
        return res;
    }

    vector<int> postorderTraversal(TreeNode* root) {
        vector<int> nodes;
        stack<TreeNode*> todo;
        TreeNode* cur = root;
        TreeNode* last = NULL;
        while (cur || !todo.empty()) {
            if (cur) {
                todo.push(cur);
                cur = cur -> left;
            } else {
                TreeNode* top = todo.top();
                if (top -> right && (last != top -> right)) {
                    cur = top -> right;
                } else {
                    nodes.push_back(top -> val);
                    last = top;
                    todo.pop();
                }
            }
        }
        return nodes;
    }
};
int main() {
	cout << "count-complete-tree-nodes/" << endl;
	return 0;
}


