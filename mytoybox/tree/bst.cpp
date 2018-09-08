#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <list>
#include <tuple>
#include "basictreenode.h"
#include "traverse_tree.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE bst
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

template<typename T>
class bst_node: public basic_tree_node
{
public:
	bst_node(const T& cc): data(cc) {}
    T data{};
    bst_node* get_left() { return static_cast<bst_node*>(left);}
    bst_node* get_right() { return static_cast<bst_node*>(right);}
};

template<typename T>
std::ostream& operator<< (std::ostream& oss, const bst_node<T>& node)
{
	oss << node.data;
	return oss;
}

template<typename T>
class bst_tree
{
public:
    typedef bst_node<T> node_type;
    ~bst_tree ();
    void set_root(node_type* nn) { root = nn; }
    template<typename Functor>
    void traverse_pre_order(Functor f){
    	::traverse_pre_order<bst_tree<T>::node_type, Functor>(f, root);
    }
    template<typename Functor>
    void traverse_in_order(Functor f){
    	::traverse_in_order<bst_tree<T>::node_type, Functor>(f, root);
    }
    template<typename Functor>
    void traverse_post_order(Functor f);

    template<typename Functor>
        void traverse_breadth_first(Functor f) {
    	::traverse_breadth_first<bst_tree<T>::node_type, Functor>(f, root);
    }

    node_type* find(T const& data)
    {
        node_type* node = root;
        while (node)
        {
            if (node->data == data)
                return node;

            if (node->data > data)
            {
                node = node->get_left();
            }
            else
                node = node->get_right();
        }
        return node;
    }

    node_type* insert(T const& data)
    {
        if (!root)
        {
            root = new node_type(data);
            return root;
        }
        node_type* node = root;
        while (true)
        {
        	if (node->data == data)
        		return node;

            if (node->data > data)
            {
                if (node->get_left())
                    node = node->get_left();
                else {
                	node_type* res = new node_type(data);
                	node->set_left(res);
                	return res;
                }
                continue;
            }

            if (node->get_right())
            	node = node->get_right();
            else {
            	node_type* res = new node_type(data);
            	node->set_right(res);
            	return res;
            }
        }
    }

private:
    node_type* root{nullptr};
};

BOOST_AUTO_TEST_CASE(check_bst)
{
    cout << "BOOST_AUTO_TEST_CASE(binary_search_tree)" << endl;
	using mytree_type = bst_tree<char>;
	mytree_type tree;
    mytree_type::node_type* anode = new mytree_type::node_type('A');
    mytree_type::node_type* bnode = new mytree_type::node_type('B');
    mytree_type::node_type* cnode = new mytree_type::node_type('C');
    mytree_type::node_type* dnode = new mytree_type::node_type('D');
    mytree_type::node_type* enode = new mytree_type::node_type('E');
    mytree_type::node_type* fnode = new mytree_type::node_type('F');
    mytree_type::node_type* gnode = new mytree_type::node_type('G');
    mytree_type::node_type* hnode = new mytree_type::node_type('H');
    mytree_type::node_type* inode = new mytree_type::node_type('I');

    tree.set_root (fnode);
    bnode = tree.insert('B');
    //fnode->left = bnode;
    fnode->right = gnode;
    bnode->left = anode;
    bnode->right = dnode;
    dnode->left = cnode;
    dnode->right = enode;
    gnode->right = inode;
    inode->left = hnode;


    BOOST_CHECK_EQUAL('A', tree_min(fnode)->data);
    BOOST_CHECK_EQUAL('C', tree_predecessor(fnode, 'D')->data);
    BOOST_CHECK_EQUAL('I', tree_max(fnode)->data);
    BOOST_CHECK_EQUAL('E', tree_successor(fnode, 'D')->data);

    for(char target='B'; target<='I'; ++target) {
    	BOOST_TEST_CONTEXT("check BST predecessor(" << target << ")")
    	BOOST_CHECK(target-1 == tree_predecessor(fnode, target)->data);

    	auto x = bst_search(fnode, target);
    	BOOST_CHECK_EQUAL(target-1, bst_predecessor(fnode, x)->data);
    }

    for(char target='A'; target<'I'; ++target) {
    	BOOST_TEST_CONTEXT("check BST successor(" << target << ")")
    	BOOST_CHECK_EQUAL(target+1, tree_successor(fnode, target)->data);

    	auto x = bst_search(fnode, target);
    	BOOST_CHECK_EQUAL(target+1, bst_successor(fnode, x)->data);
    }
}

auto sorted_list_to_bst(list<int>::iterator beg, int n)
{
    if (n <= 0)
        return std::make_tuple(beg, (bst_node<int>*) nullptr);
    auto [lbeg, lroot] = sorted_list_to_bst(beg, n / 2);
    auto root = new bst_node<int>(*lbeg);
    root->left = lroot;
    auto [rbeg, rroot] = sorted_list_to_bst(++lbeg, n - 1 - n / 2);
    root->right = rroot;
    return std::make_tuple(rbeg, root);
}

void inorder(bst_node<int>* root, list<int>& out)
{
    if (!root)
        return;
    inorder(root->get_left(), out);
    out.push_back(root->data);
    inorder(static_cast<bst_node<int>*>(root->right), out);
}

int depth(basic_tree_node* root)
{
    if (!root)
        return 0;
    return 1 + max(depth(root->left), depth(root->right));
}

BOOST_AUTO_TEST_CASE(check_sorted_list_to_bst)
{
    cout << "BOOST_AUTO_TEST_CASE(sorted_list_to_bst)" << endl;
    list<int> nums;
    for (int i = 0; i < 15; ++i)
        nums.push_back(i);

    auto [iter, root] = sorted_list_to_bst(nums.begin(), nums.size());

    BOOST_CHECK(iter == nums.end());
    list<int> dst;
    inorder(static_cast<bst_node<int>*>(root), dst);
    copy(dst.begin(), dst.end(), ostream_iterator<int>(cout, " "));
    cout << "\n";
    BOOST_CHECK(std::equal(nums.begin(), nums.end(), dst.begin()));
    BOOST_CHECK_EQUAL(4, depth(root));
}

template<typename T> template<typename Functor>
void bst_tree<T>::traverse_post_order(Functor f){
	::traverse_post_order<bst_tree<T>::node_type, Functor>(f, root);
}

template<typename T>
bst_tree<T>::~bst_tree ()
{
	delete root;
	root = nullptr;
}

