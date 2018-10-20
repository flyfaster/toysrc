#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <stack>
#include <vector>

using namespace std;

struct TreeNode
{
	TreeNode(int n) : data(n) {}
    int data = 0;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

/* traverse binary tree without recursion and without stack */
void MorrisInorderTraversal(TreeNode* root, std::function<void(TreeNode*)> visit_func)
{
	if (root == nullptr)
		return;

	auto current = root;
	TreeNode *pre = nullptr;

	while (current) {
		if (!current->left) {
			visit_func(current);
			current = current->right;
		}
		else {
			/* Find the inorder predecessor of current */
			pre = current->left;
			while (pre->right && pre->right != current)
				pre = pre->right;

			/* Make current as right child of its inorder
			predecessor */
			if (pre->right == nullptr) {
				pre->right = current;
				current = current->left;
			}

			/* Revert the changes made in if part to restore
			the original tree i.e., fix the right child
			of predecessor */
			else {
				pre->right = nullptr;
				visit_func(current);
				current = current->right;
			} /* End of if condition pre->right == nullptr */
		} /* End of if condition current->left == nullptr*/
	} /* End of while */
}

// Preorder traversal without recursion and without stack
void MorrisPreorderTraversal(TreeNode* root, std::function<void(TreeNode*)> visitor)
{
    while (root)
    {
        // If left child is null, print the current node data. Move to
        // right child.
        if (!root->left)
        {
        	visitor(root);
            root = root->right;
        }
        else
        {
            // Find inorder predecessor
            TreeNode* current = root->left;
            while (current->right && current->right != root)
                current = current->right;

            // If the right child of inorder predecessor already points to
            // this node
            if (current->right == root)
            {
                current->right = nullptr;
                root = root->right;
            }

            // If right child doesn't point to this node, then print this
            // node and make right child point to this node
            else
            {
            	visitor(root);
                current->right = root;
                root = root->left;
            }
        }
    }
}

TreeNode* construct_bst_preorder(vector<int> const& pre)
{
	if (pre.empty())
		return nullptr;

    stack<TreeNode*> st;
    TreeNode* root = new TreeNode(pre[0]);
    st.push(root);
    for (int i = 1; i < pre.size(); ++i)
    {
        TreeNode* temp = nullptr;
        /* Keep on popping while the next value is greater than
           stack's top value. */
        while (!st.empty() && pre[i] > st.top()->data)
        {
            temp = st.top();
            st.pop();
        }
        // Make this greater value as the right child and push it to the stack
        if (temp != nullptr)
        {
            temp->right = new TreeNode(pre[i]);
            st.push(temp->right);
        }
        // If the next value is less than the stack's top value, make this value
        // as the left child of the stack's top node. Push the new node to stack
        else
        {
            st.top()->left = new TreeNode(pre[i]);
            st.push(st.top()->left);
        }
    }
    return root;
}

TreeNode* bst_from_sorted_vec(vector<int> const& sorted, int i, int j)
{
    if (i >= j)
        return nullptr;
    int mid = i + (j - i) / 2;
    TreeNode* root = new TreeNode(sorted[mid]);
    root->left = bst_from_sorted_vec(sorted, i, mid);
    root->right = bst_from_sorted_vec(sorted, mid + 1, j);
    return root;
}

void release_tree(TreeNode* root)
{
	if (root)
	{
		release_tree(root->left);
		release_tree(root->right);
		delete root;
	}
}

int tree_depth(TreeNode* root)
{
    if (!root)
        return 0;
    return 1 + max(tree_depth(root->left), tree_depth(root->right));
}

void recursive_pre(TreeNode* root, std::function<void(TreeNode*)> visitor)
{
	if (root)
	{
		visitor(root);
		recursive_pre(root->left, visitor);
		recursive_pre(root->right, visitor);
	}
}

void recursive_inorder(TreeNode* root, std::function<void(TreeNode*)> visitor)
{
	if (root)
	{
		recursive_inorder(root->left, visitor);
		visitor(root);
		recursive_inorder(root->right, visitor);
	}
}

int main()
{
	vector<int> nums(31);
	std::iota(nums.begin(), nums.end(), 1);
//	std::random_device rd;
//	std::mt19937 g(rd());
//	std::shuffle(nums.begin(), nums.end(), g);

	TreeNode* root = bst_from_sorted_vec(nums, 0, nums.size());
	cout << "tree depth " << tree_depth(root) << "\n";

	vector<int> vi;
	auto to_vec = [&vi](TreeNode* root) {
		vi.push_back(root->data);
	};

	recursive_inorder(root, to_vec);
	copy(vi.begin(), vi.end(), ostream_iterator<int>(cout, " "));
	std::cout << "\n";
	vi.clear();

	MorrisInorderTraversal(root, to_vec);
	copy(vi.begin(), vi.end(), ostream_iterator<int>(cout, " "));
	std::cout << "\n";

	vi.clear();
	recursive_pre(root, to_vec);
	release_tree(root);

	auto pre_vi = vi;
	root = construct_bst_preorder(vi);
	vi.clear();

	MorrisPreorderTraversal(root, to_vec);
	copy(vi.begin(), vi.end(), ostream_iterator<int>(cout, " "));
	std::cout << "\n";
	copy(pre_vi.begin(), pre_vi.end(), ostream_iterator<int>(cout, " "));
	std::cout << "\n";

	vi.clear();

	MorrisInorderTraversal(root, to_vec);
	copy(vi.begin(), vi.end(), ostream_iterator<int>(cout, " "));
	std::cout << "\n";
	release_tree(root);
	return 0;
}

// valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all bld/morris
