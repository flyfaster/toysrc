/**
 *  * Definition for a binary tree node.
 *   * struct TreeNode {
 *    *     int val;
 *     *     TreeNode *left;
 *      *     TreeNode *right;
 *       *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 *        * };
 *         */
class Solution {
public:
    vector<string> binaryTreePaths(TreeNode* root) {
    	vector<string> ret;
    	unordered_map<TreeNode*, TreeNode*> lookup_parent;// key: child, value: parent
    	unordered_set<TreeNode*> leaves;  // leaf nodes without children
    	list<TreeNode*> nodestack;
    	TreeNode* pnode = root;
    	lookup_parent[root] = nullptr;
    	while(pnode || nodestack.size()) {
    		if(pnode) {
    			if(pnode->right) {
    				lookup_parent[pnode->right] = pnode;
    				nodestack.push_back(pnode->right);
    			}
    			if(pnode->left)
    				lookup_parent[pnode->left] = pnode;
    			if(pnode->left == pnode->right) {
    				leaves.insert(pnode);
    			}
    			pnode = pnode->left;
    		} else {
    			pnode = *nodestack.rbegin();
    			nodestack.pop_back();
    		}
    	}
    	for(auto leaf: leaves) {
    		list<TreeNode*> path;
    		TreeNode* pnode = leaf;
    		do {
    			path.push_front(pnode);
    			pnode = lookup_parent[pnode];
    		} while(pnode);
    		std::stringstream ss;
    		ss << path.front()->val;
    		path.pop_front();
    		for(auto anode: path)
    			ss<<"->" <<anode->val;
    		ret.push_back(ss.str());
    	}
    	return ret;
    }
};
