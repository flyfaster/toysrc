#include <iostream>
#include <iomanip>
#include <functional>

struct TreeNode
{
	TreeNode(int n) : data(n) {}
    int data = 0;
    TreeNode* left = nullptr;
    TreeNode* right = nullptr;
};

void visit(TreeNode* node)
{
	std::cout << node->data << " ";
}
/* Function to traverse binary tree without recursion and
without stack */
void MorrisInorderTraversal(TreeNode* root, std::function<void(TreeNode*)> visit_func)
{
	if (root == nullptr)
		return;

	auto current = root;
	TreeNode *pre = nullptr;

	while (current) {
		if (current->left == nullptr) {
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

int main()
{
/* Constructed binary tree is
		  1
		 / \
		2	3
	   / \
	  4	  5
*/
	TreeNode* root = new TreeNode(1);
	root->left = new TreeNode(2);
	root->right = new TreeNode(3);
	root->left->left = new TreeNode(4);
	root->left->right = new TreeNode(5);

	MorrisInorderTraversal(root, visit);
	std::cout << "\n";
	return 0;
}
