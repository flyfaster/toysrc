#include <vector>
#include <stack>
#include <list>
using namespace std;
struct TreeNode{
	int data;
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent;
	int size;
};

vector<int> inorder(TreeNode* root)
{
    stack<TreeNode*> st;
    vector<int> res;
    auto curr = root;
    while (!st.empty() || curr)
    {
        if (curr)
        {
            st.push(curr);
            curr = curr->left;
        }
        else
        {
            curr = st.top();
            st.pop();
            res.push_back(curr->data);
            curr = curr->right;
        }
    }
    return res;
}

vector<int> preorder(TreeNode* root)
{
    vector<int> res;
    stack<TreeNode*> st;
    st.push_back(root);
    while (!st.empty())
    {
        auto curr = st.top();
        st.pop();
        if (curr)
        {
            res.push_back(curr->data);
            st.emplace(curr->right);
            st.emplace(curr->left);
        }
    }
    return res;
}

TreeNode* find_kth(TreeNode* root, int k)
{
    auto iter = root;
    while (iter)
    {
        int left_size = iter->left ? iter->left->size : 0;
        if (left_size + 1 < k)
        {
            k -= left_size + 1;
            iter = iter->right;
        }
        else if (left_size == k - 1)
            return root;
        iter = iter->left;
    }
    return nullptr;
}

TreeNode* successor(TreeNode* root){
	auto node = root;
	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;
		return node;
	}
	while (node->parent && node->parent->right == node)
		node = node->parent;

	return node->parent;
}

vector<int> inorder_space_o1(TreeNode* root)
{
    auto prev = root;
    auto curr = root;
    vector<int> res;

    while (curr)
    {
        auto next = curr; // just initialize next to dummy value
        if (curr->parent == prev)
        {
            if (curr->left)
                next = curr->left;
            else
            {
                res.push_back(curr->data);
                next = (curr->right) ? curr->right : curr->parent;
            }
        }
        else if (curr->left == prev)
        {
            res.push_back(curr->data);
            next = (curr->right) ? curr->right : curr->parent;
        }
        else
            next = curr->parent;
        prev = curr;
        curr = next;
    }
    return res;
}

std::list<TreeNode*> list_of_leaves(TreeNode* root)
{
    std::list<TreeNode*> res;
    if (root)
    {
        if (!root->left && !root->right)
            res.emplace_back(root);
        else
        {
            res.splice(res.end(), tree_to_list(root->left));
            res.splice(res.end(), tree_to_list(root->right));
        }
    }
    return res;
}
