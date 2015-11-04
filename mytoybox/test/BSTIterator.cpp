//============================================================================
// Name        : BSTIterator.cpp
//============================================================================
#include <stack>
#include <iostream>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class BSTIterator {
public:
	std::stack<TreeNode*> nodestack;
	void addleft(TreeNode* root) {
	    while(root!=nullptr) {
    		nodestack.push(root);
    		root=root->left;
    	}
	}
    BSTIterator(TreeNode *root) {
    	addleft(root);
    }

    /** @return whether we have a next smallest number */
    bool hasNext() {
    	return (!nodestack.empty());
    }

    /** @return the next smallest number */
    int next() {
    	int ret = nodestack.top()->val;
    	TreeNode* node = nodestack.top();
    	nodestack.pop();
    	addleft(node->right);
    	return ret;
    }
};

int main() {
	TreeNode* root = new TreeNode(1);
	BSTIterator i = BSTIterator(root);
	while (i.hasNext()) cout << i.next();
	cout << "Submission Result: Accepted " << endl;
	return 0;
}

