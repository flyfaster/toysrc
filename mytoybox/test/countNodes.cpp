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
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
class Solution {
public:
	int leftHeight(TreeNode* root) {
		int height = 0;
		while(root) {
			height++;
			root = root->left;
		}
		return height;
	}
	int rightHeight(TreeNode* root) {
		int height = 0;
		while(root) {
			height++;
			root = root->right;
		}
		return height;
	}
    int countNodes(TreeNode* root) {
    	if(root==nullptr) return 0;
    	if(root->left == nullptr && root->right == nullptr) return 1;
    	int leftheight = leftHeight(root);
    	int rightheight = rightHeight(root);
    	if (leftheight == rightheight) return pow(2, leftheight)-1;
    	return 1+countNodes(root->left)+countNodes(root->right);
    }
};
int main() {
	cout << "count-complete-tree-nodes/" << endl;
	return 0;
}


