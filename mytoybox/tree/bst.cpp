#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include "basictreenode.h"
#include "traverse_tree.h"
using namespace std;

template<typename T>
class bst_node: public basic_tree_node
{
public:
	bst_node(const T& cc): data(cc) {}
	bst_node(): data(0) {}
    T data;
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
    bst_tree ();
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
private:
	node_type *root;
};

int main (int argc, char* argv[])
{
	cout << argv[0] << " start\n";
	typedef bst_tree<char> mytree_type;
	mytree_type tree;
    mytree_type::node_type* anode = new mytree_type::node_type('A');
    mytree_type::node_type* bnode = new mytree_type::node_type('B');
    mytree_type::node_type* cnode = new mytree_type::node_type('C');
    mytree_type::node_type* dnode = new mytree_type::node_type('D');
    mytree_type::node_type* enode = new mytree_type::node_type('E');
    mytree_type::node_type* fnode = new mytree_type::node_type('F');
    mytree_type::node_type* gnode = new mytree_type::node_type('G');
    mytree_type::node_type* inode = new mytree_type::node_type('I');
    mytree_type::node_type* hnode = new mytree_type::node_type('H');

    tree.set_root (fnode);
    fnode->left = bnode;
    fnode->right = gnode;
    bnode->left = anode;
    bnode->right = dnode;
    dnode->left = cnode;
    dnode->right = enode;
    gnode->right = inode;
    inode->left = hnode;

    auto print_to_stdout=[](mytree_type::node_type& node){ std::cout<<(node) << " ";};
    cout << "Traversal By Preorder:\n";
    tree.traverse_pre_order (print_to_stdout);
    cout << endl;
    cout << "Traversal By Inorder:\n";
    tree.traverse_in_order (print_to_stdout);
    cout << endl;
    cout << "Traversal By Postorder:\n";
    tree.traverse_post_order (print_to_stdout);
    cout << endl;
    cout << "Breadth first traverse:\n";
    tree.traverse_breadth_first (print_to_stdout);
    cout << std::endl;
    cout << "check memory leak\n";
    cout << "valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes "
    		<< argv[0] << std::endl;
    cout << argv[0] << " done\n";
    return 0;
}

template<typename T> template<typename Functor>
void bst_tree<T>::traverse_post_order(Functor f){
	::traverse_post_order<bst_tree<T>::node_type, Functor>(f, root);
}

template<typename T>
bst_tree<T>::bst_tree ()
{
	root = nullptr;
}

template<typename T>
bst_tree<T>::~bst_tree ()
{
	delete root;
	root = nullptr;
}

