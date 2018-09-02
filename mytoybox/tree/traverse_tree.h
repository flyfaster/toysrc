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
#include <cassert>
template<typename node_type, typename Functor>
void traverse_pre_order(Functor f, basic_tree_node* root){
	// http://en.wikipedia.org/wiki/Tree_traversal#Pre-order
	using NodeStack = std::stack<node_type*>;
	NodeStack parentStack;
	node_type *node = static_cast<node_type*>( root);
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
	node_type *node = static_cast<node_type*>( root);
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

template <typename node_type, typename Functor>
void traverse_breadth_first(Functor f, basic_tree_node* root)
{ // http://en.wikipedia.org/wiki/Tree_traversal#Breadth-first_2
    std::queue<node_type*> Q;
    if (root == nullptr)
        return;
    Q.push(static_cast<node_type*>(root));
    while (!Q.empty())
    {
        node_type* p = Q.front();
        Q.pop();
        f(*p);
        if (p->get_left())
            Q.push(p->get_left());
        if (p->get_right())
            Q.push(p->get_right());
    }
}

template <typename node_type>
node_type* tree_min(node_type* root)
{
    while (root && root->get_left())
        root = root->get_left();
    return root;
}

template <typename node_type>
node_type* tree_max(node_type* root)
{
    while (root && root->get_right())
        root = root->get_right();
    return root;
}

template <typename node_type>
node_type* tree_parent(node_type* root, node_type* target)
{
    if (root == nullptr || root == target)
        return nullptr;
    node_type* parent = root;
    while (root->data != target->data)
    {
        parent = root;
        if (root->data > target->data)
            root = root->get_left();
        else
            root = root->get_right();
    }
    return parent;
}

template <typename node_type>
node_type* bst_predecessor(node_type* root, node_type* target)
{ // http://www.crazyforcode.com/inorder-predecessor-binary-search-tree/
	if (target->get_left())
		return tree_max(target->get_left());

	node_type* predecessor = nullptr;
	while (root) {
		if (target->data > root->data) {
			predecessor = root;
			root = root->get_right();
		} else if (target->data < root->data) {
			root = root->get_left();
		} else
			break; // found target
	}
	return predecessor;
}

template <typename node_type>
node_type* bst_successor(node_type* root, node_type* target)
{ // http://www.crazyforcode.com/inorder-successor-binary-search-tree/
	if (target->get_right())
		return tree_min(target->get_right());

	node_type* successor = nullptr;
	while (root) {
		if (target->data < root->data) {
			successor = root;
			root = root->get_left();
		} else if (target->data > root->data) {
			root = root->get_right();
		} else
			break; // found target
	}
	return successor;
}

template <typename node_type, typename T>
node_type* bst_search(node_type* root, T const& target)
{
    std::stack<node_type*> parentStack;
    while (root && root->data != target)
    {
        parentStack.push(root);
        if (root->data > target)
            root = root->get_left();
        else
            root = root->get_right();
    }
    return root;
}

template <typename node_type, typename T>
std::stack<node_type*> bst_search_path(node_type* root, T const& target)
{
    std::stack<node_type*> path_stack;
    while (root && root->data != target)
    {
        path_stack.push(root);
        if (root->data > target)
            root = root->get_left();
        else
            root = root->get_right();
    }

    if (root && root->data == target)
        path_stack.push(root);
    else
        return std::stack<node_type*>{}; // return empty stack if target is not found
    return path_stack;
}

template <typename node_type, typename T>
node_type* tree_predecessor(node_type* root, T const& target)
{
    auto path_stack = bst_search_path(root, target);
    if (path_stack.empty())
        return nullptr;
    auto target_node = path_stack.top();
    path_stack.pop();

    if (target_node->get_left())
        return tree_max(target_node->get_left());
    assert(target_node->data == target);

    // if it does not have left child, predecessor is its first left ancestor
    auto parent = path_stack.empty() ? nullptr : path_stack.top();
    while (parent && parent->get_left() == target_node)
    {
        target_node = parent;
        path_stack.pop();
        parent = path_stack.empty() ? nullptr : path_stack.top();
    }
    return parent;
}

template <typename node_type, typename T>
node_type* tree_successor(node_type* root, T const& target)
{
    std::stack<node_type*> parentStack;
    while (root && root->data != target)
    {
        parentStack.push(root);
        if (root->data < target)
            root = root->get_right();
        else
            root = root->get_left();
    }

    if (!root) // target is not in the tree
        return nullptr;

    if (root->get_right())
        return tree_min(root->get_right());

    node_type* parent = parentStack.empty() ? nullptr : parentStack.top();
    while (parent && parent->get_right() == root)
    {
        root = parent;
        parentStack.pop();
        if (!parentStack.empty())
        {
            parent = parentStack.top();
        }
    }

    return parent;
}

#endif /* TRAVERSE_TREE_H_ */
