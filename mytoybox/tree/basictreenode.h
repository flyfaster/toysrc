/*
 * basictreenode.h
 *
 *  Created on: Feb 21, 2015
 *      Author: onega
 */

#ifndef BASICTREENODE_H_
#define BASICTREENODE_H_
#include <stack>
class basic_tree_node {
public:
	basic_tree_node();
	virtual ~basic_tree_node();
	basic_tree_node* left;
	basic_tree_node* right;
    void set_left(basic_tree_node* nn) { left = nn; }
    void set_right(basic_tree_node* nn) { right = nn; }
    bool is_leaf() { return left==nullptr && right == nullptr; }
    void disconnect_children() { left = nullptr; right = nullptr; }
    void disconnect_child(basic_tree_node* nn) {
    	if (left==nn)
    		left = nullptr;
    	if (right == nn)
    		right = nn;
    }
};



#endif /* BASICTREENODE_H_ */
