/*
 * traverse_tree.h
 *
 *  Created on: Feb 21, 2015
 *      Author: onega
 */

#ifndef TRAVERSE_TREE_H_
#define TRAVERSE_TREE_H_
#include "basictreenode.h"
#include <stack>
#include <queue>

template<typename node_type, typename Functor>
void traverse_pre_order(Functor f, basic_tree_node* root){
	// http://en.wikipedia.org/wiki/Tree_traversal#Pre-order
	typedef std::stack<node_type*> NodeStack;
	NodeStack parentStack;
	node_type *node = static_cast<node_type*>( root);;
	while (!parentStack.empty() || node != nullptr) {
		if (node != nullptr) {
			f(*node);
			if (node->get_right())
				parentStack.push(node->get_right());
			node = node->get_left();
		} else {
			node = parentStack.top();
			parentStack.pop();
		}
	}
}

template<typename node_type, typename Functor>
void traverse_in_order(Functor f, basic_tree_node* root){
	// http://en.wikipedia.org/wiki/Tree_traversal#In-order
	typedef std::stack<node_type*> NodeStack;
	NodeStack parentStack;
	node_type *node = static_cast<node_type*>( root);;
	while (!parentStack.empty() || node != nullptr) {
		if (node != nullptr) {
			parentStack.push(node);
			node = node->get_left();
		} else {
			node = parentStack.top();
			parentStack.pop();
			f(*node);
			node = node->get_right();
		}
	}
}

template<typename node_type, typename Functor>
void traverse_post_order(Functor f, basic_tree_node* root){
	typedef std::stack<node_type*> NodeStack;
	NodeStack parentStack;
	node_type * lastnodevisited = nullptr;
	node_type *node = static_cast<node_type*>( root);
	while (!parentStack.empty() || node != nullptr) {
		if (node != nullptr) {
			parentStack.push(node);
			node = node->get_left();
		} else {
			node_type *peeknode = parentStack.top();
			if (peeknode->get_right() != nullptr
					&& lastnodevisited != peeknode->get_right()) {
				/* if right child exists AND traversing node from left child, move right */
				node = peeknode->get_right();
			} else {
				f(*peeknode);
				lastnodevisited = parentStack.top();
				parentStack.pop();
			}
		}
	}
}

template<typename node_type, typename Functor>
void traverse_breadth_first(Functor f, basic_tree_node* root)
{ // http://en.wikipedia.org/wiki/Tree_traversal#Breadth-first_2
	std::queue<node_type*> Q;
    if (root == nullptr)
        return;
    Q.push (static_cast<node_type*>(root));
    while (!Q.empty ())
    {
    	node_type *p;
        p = Q.front ();
        Q.pop ();
        f(*p);
        if (p -> get_left())
            Q.push (p -> get_left());
        if (p -> get_right())
            Q.push (p -> get_right());
    }
}


#endif /* TRAVERSE_TREE_H_ */
