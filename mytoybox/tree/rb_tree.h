#ifndef RB_TREE_H_
#define RB_TREE_H_
#include <limits>
#include <exception>
#include <iostream>
#include <stdexcept>
#include "traverse_tree.h"
// http://www.kurttest.com/cplusplus/notes/rb.html
#include "basictreenode.h"
class rb_exception :  public std::exception {
public:
  virtual const char* what() const throw()
  {
    return "Key does not exist";
  }
};

template<typename Key, typename Value> class rb_tree {
private:
	enum class Color { // http://www.cprogramming.com/c++11/c++11-nullptr-strongly-typed-enum-class.html
		RED, BLACK
	};
public:
	class rb_node: public basic_tree_node {
	private:
		Color color;           // color of parent link
	public:
		Key key;
		Value value;
		rb_node * get_left() { return static_cast<rb_node*>(left); }
		rb_node * get_right() { return static_cast<rb_node*>(right); }
		rb_node* parent;
		rb_node(Key pkey, Value pvalue) :
				key(pkey), value(pvalue), color(Color::RED) {
		}
		bool red() const {
			return color == Color::RED;
		}
		bool black() const {
			return color == Color::BLACK;
		}
		void flip() {
			if (red())
				color = Color::BLACK;
			else
				color = Color::RED;
		}
		void set_red() {
			color = Color::RED;
		}
		void set_black() {
			color = Color::BLACK;
		}
		void set_color(rb_node* nn) {
			color = nn->color;
		}
		rb_node *grandparent()
		{
		 if (parent != nullptr)
		  return parent->parent;
		 else
		  return nullptr;
		}

		rb_node *uncle()
		{
			rb_node *g = grandparent();
		 if (g == nullptr)
		  return nullptr; // No grandparent means no uncle
		 if (parent == g->left)
		  return g->right;
		 else
		  return g->left;
		}
		rb_node *sibling()
		{
			if(parent == nullptr)
				return nullptr;
		 if (this == parent->get_left())
		  return parent->get_right();
		 else
		  return parent->get_left();
		}
	};
	rb_node *root;
	Value get(rb_node *p, Key key) throw (rb_exception);
	rb_node *getInOrderSuccessorNode(rb_node *p);
	/*
	 * Returns minimum key of subtree rooted at p
	 */
	Key min(rb_node *p) {
		if (p==nullptr)
			throw std::out_of_range( " nullptr");
		return (p->left == nullptr) ? p->key : min(p->left);
	}

	Key max(rb_node *p) {
		return (p->right == nullptr) ? p->key : max(p->right);
	}

	rb_node *insert(rb_node *p, Key key, Value value);
	void insert(Key key, Value value)
	{
		rb_node *parent, *next=root;
		if(root == nullptr) {
			root = new rb_node(key, value);
			root->set_black();
			return;
		}
		while(next!=nullptr) {
			if(key == next->key) {
				next->value = value;
				return;
			}
			parent = next;
			if (key <next->key) {
				next = next->get_left();
			}
			else {
				next = next->get_right();
			}
		}
		rb_node *p = new rb_node(key, value);
		p->set_red();
		if(key<parent->key)
			parent->set_left(p);
		else
			parent->set_right(p);
		insert_case1(p);
	}
	void insert_case1(rb_node *n)
	{
	 if (n->parent == NULL)
	  n->set_black();
	 else
	  insert_case2(n);
	}
	void insert_case2(rb_node *n)
	{
	 if (n->parent->black())
	  return; /* Tree is still valid */
	 else
	  insert_case3(n);
	}
	void insert_case3(rb_node *n)
	{
		rb_node *u = n->uncle(), *g;
	 if ((u != NULL) && (u->red())) {
	  n->parent->set_black();
	  u->set_black();
	  g = n->grandparent();
	  g->set_red;
	  insert_case1(g);
	 } else {
	  insert_case4(n);
	 }
	}
	void insert_case4(rb_node *n)
	{
		rb_node *g = n->grandparent();

	 if ((n == n->parent->right) && (n->parent == g->left)) {
	  rotate_left(n->parent);

	 /*
	 * rotate_left can be the below because of already having *g =  grandparent(n)
	 *
	 * struct node *saved_p=g->left, *saved_left_n=n->left;
	 * g->left=n;
	 * n->left=saved_p;
	 * saved_p->right=saved_left_n;
	 *
	 * and modify the parent's nodes properly
	 */

	  n = n->left;

	 } else if ((n == n->parent->left) && (n->parent == g->right)) {
	  rotate_right(n->parent);

	 /*
	 * rotate_right can be the below to take advantage of already having *g =  grandparent(n)
	 *
	 * struct node *saved_p=g->right, *saved_right_n=n->right;
	 * g->right=n;
	 * n->right=saved_p;
	 * saved_p->left=saved_right_n;
	 *
	 */

	  n = n->right;
	 }
	 insert_case5(n);
	}
	void insert_case5(rb_node *n)
	{
		rb_node *g = n->grandparent();

	 n->parent->set_black();
	 g->set_red();
	 if (n == n->parent->left)
	  rotate_right(g);
	 else
	  rotate_left(g);
	}
	bool is_red(rb_node *p) {
		return (p == nullptr) ? false : (p->red());
	}

	void flip_color(rb_node *p) {
		if (p == nullptr) {
			return;
		}
		p->flip();
		p->get_left()->flip();
		p->get_right()->flip();
	}

	rb_node *rotate_left(rb_node *p);
	rb_node *rotate_right(rb_node *p);

	rb_node *moveRedLeft(rb_node *p);
	rb_node *moveRedRight(rb_node *p);

	rb_node *deleteMax(rb_node *p);
	rb_node *deleteMin(rb_node *p);

	rb_node *fixUp(rb_node *p);

	rb_node *remove(rb_node *p, Key key);

public:

	rb_tree() {
		root = nullptr;
	}
	~rb_tree();

	bool contains(Key key) {
		rb_node* p = root;
		while (p != nullptr) {
			if (key < p->key)
				p = p->get_left();
			else if (key > p->key)
				p = p->get_right();
			else
				return true;
		}
		return false;
	}

	Value get(Key key) {
		return get(root, key);
	}

	void put(Key key, Value value) {
		root = insert(root, key, value);
		root->set_black();
	}

	template<typename Functor> void traverse(Functor f);

	Key min() {
		if (root==nullptr)
			throw std::out_of_range( " nullptr");
		return (root == nullptr) ? 0 : min(root);
	}

	Key max() {
		if (root==nullptr)
			throw std::out_of_range( " nullptr");
		return (root == nullptr) ? 0 : max(root);
	}

	void deleteMin() {
		root = deleteMin(root); // it takes care of nullptr
		if (root == nullptr) {
			return;
		}
		root->set_black();
	}

	void deleteMax() {
		root = deleteMax(root); // it takes care of nullptr
		if (root == nullptr) {
			return;
		}
		root->set_black();
	}

	void remove_notwork(Key key) {
		root = remove(root, key); // it takes care of nullptr
		if (root == nullptr) {
			return;
		}
		root->set_black();
	}
	void remove(Key key) {
		rb_node* next, *parent;
		next = root;
		while(next!=nullptr) {
			if (next->key == key)
			{
				break;
			}
			if (key < next->key)
				next = next->get_left();
			else
				next = next->get_right();
		}
		if (next == nullptr) {
			return; // key not found
		}
		if (next->get_left() && next->get_right()) {
			rb_node *successor = getInOrderSuccessorNode(next);
			next->key = successor->key;
			next->value = successor->value;
			next = successor;
		}
		delete_one_child(next);
	}

	void delete_one_child(rb_node *n) {
		/*
		 * Precondition: n has at most one non-null child.
		 */
		rb_node *child = n->get_left() ? n->get_left() : n->get_right();

		replace_node(n, child);
		if (n->black()) {
			if (is_red(child)) // child may be nullptr
				child->set_black();
			else
				delete_case1(child);
		}
		delete (n);
	}
	void delete_case1(rb_node *n)
	{
		if(n == nullptr)
			return;
	 if (n->parent != NULL)
	  delete_case2(n);
	}
	void delete_case2(rb_node *n)
	{
		rb_node *s = n->sibling();

	 if (s->red()) {
	  n->parent->set_red();
	  s->set_black();
	  if (n == n->parent->get_left())
	   rotate_left(n->parent);
	  else
	   rotate_right(n->parent);
	 }
	 delete_case3(n);
	}
	void delete_case3(rb_node *n)
	{
		rb_node *s = n->sibling();

	 if ((n->parent->black()) &&
	     (s->black()) &&
	     (s->get_left()->black()) &&
	     (s->get_right()->black())) {
	  s->set_red();
	  delete_case1(n->parent);
	 } else
	  delete_case4(n);
	}
	void delete_case4(rb_node *n)
	{
		rb_node *s = n->sibling();

	 if ((n->parent->red()) &&
	     (s->black()) &&
	     (s->get_left()->black()) &&
	     (s->get_right()->black())) {
	  s->set_red();
	  n->parent->set_black();
	 } else
	  delete_case5(n);
	}
	void delete_case5(rb_node *n)
	{
		rb_node *s = n->sibling();

	 if  (s->black()) { /* this if statement is trivial,
	due to case 2 (even though case 2 changed the sibling to a sibling's child,
	the sibling's child can't be red, since no red parent can have a red child). */
	/* the following statements just force the red to be on the left of the left of the parent,
	   or right of the right, so case six will rotate correctly. */
	  if ((n == n->parent->left) &&
	      (s->get_right()->black()) &&
	      (s->get_left()->red())) { /* this last test is trivial too due to cases 2-4. */
	   s->set_red();
	   s->get_left()->set_black();
	   rotate_right(s);
	  } else if ((n == n->parent->right) &&
	             (s->get_left()->black()) &&
	             (s->get_right()->red())) {/* this last test is trivial too due to cases 2-4. */
	   s->set_red();
	   s->get_right()->set_black();
	   rotate_left(s);
	  }
	 }
	 delete_case6(n);
	}
	void delete_case6(rb_node *n)
	{
		rb_node *s = n->sibling();

	 s->set_color ( n->parent);
	 n->parent->set_black();

	 if (n == n->parent->get_left()) {
	  s->get_right()->set_black();
	  rotate_left(n->parent);
	 } else {
	  s->get_left()->set_black();
	  rotate_right(n->parent);
	 }
	}
	void replace_node(rb_node *n, rb_node *child){
		if (child) {
			child->parent = n->parent;
		}
		n->disconnect_children();
		if(n->parent == nullptr) {
			root = child;
			return;
		}
		if (n == n->parent->get_left()) {
			n->parent->set_left(child);
		} else {
			n->parent->set_right(child);
		}
	}
};

template<typename Key, typename Value> inline rb_tree<Key, Value>::~rb_tree()
{
	delete root;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::rotate_left(rb_node *p)
{  // Make a right-leaning 3-node lean to the left.
	if (p == nullptr) {
		return nullptr;
	}
   rb_node  *x = p->get_right();
   p->right = x->left;
   x->left  = p;
   x->set_color (x->get_left());
   x->get_left()->set_red();
   return x;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node * rb_tree<Key, Value>::rotate_right(rb_node *p)
{  // Make a left-leaning 3-node lean to the right.
	if (p == nullptr) {
		return nullptr;
	}
   rb_node *x = p->get_left();
   p->left = x->right;
   x->right = p;
   x->set_color(x->get_right());
   x->get_right()->set_red();
   return x;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node * rb_tree<Key, Value>::moveRedLeft(rb_node *p)
{  // Assuming that p is red and both p->left and p->left->left
   // are black, make p->left or one of its children red
	if (p == nullptr) {
		return nullptr;
	}
   flip_color(p);
   if (is_red(p->get_right()->get_left())) {
      p->right = rotate_right(p->get_right());
      p = rotate_left(p);
      flip_color(p);
   }
  return p;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node * rb_tree<Key, Value>::moveRedRight(rb_node *p)
{  // Assuming that p is red and both p->right and p->right->left
   // are black, make p->right or one of its children red
	if (p == nullptr) {
		return nullptr;
	}
	flip_color(p);
	// what if p->left == nullptr?
   if (is_red(p->get_left()->get_left()))
   {
      p = rotate_right(p);
      flip_color(p);
   }
   return p;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::fixUp(rb_node *p)
{
	if (p == nullptr) {
		return nullptr;
	}
   if (is_red(p->get_right())) {
      p = rotate_left(p);
   }

   if (is_red(p->get_left()) && is_red(p->get_left()->get_left()))
      p = rotate_right(p);

   if (is_red(p->get_left()) && is_red(p->get_right())) // four node
      flip_color(p);

   return p;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::deleteMax(rb_node *p)
{
	if (p == nullptr) {
		return nullptr;
	}
   if (is_red(p->get_left()))
      p = rotate_right(p);

   if (p->right == nullptr) {
	   // what if tree has single node?
      return nullptr;
   }

   if (!is_red(p->right) && !is_red(p->right->left))
      p = moveRedRight(p);

   p->right = deleteMax(p->right);

   return fixUp(p);
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::deleteMin(rb_node *p)
{
	if (p == nullptr) {
		return nullptr;
	}
   if (p->left == nullptr) {
	   // should I delete one node? in this case, does tree has right node?
      return nullptr;
   }

   if (!is_red(p->get_left()) && !is_red(p->get_left()->get_left())) {
      p = moveRedLeft(p);
   }
   rb_node *ptemp = p->get_left();
   p->left = deleteMin(p->get_left());
   ptemp->disconnect_children();
   delete ptemp;
   return fixUp(p);
}

// http://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal
template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::remove(rb_node *p, Key key)
{// not working
	// need to check http://opendatastructures.org/ods-cpp.pdf
	if (p == nullptr) {
		return nullptr;
	}
   if (key < p->key) {
      if (!is_red(p->get_left()) && !is_red(p->get_left()->get_left())) {
         p = moveRedLeft(p);
      }

      p->left = remove(p->get_left(), key);

   } else {
      if (is_red(p->get_left())) {
         p = rotate_right(p);
      }

      if ((key == p->key) && (p->right == nullptr)) {
         return nullptr;
      }

      if (!is_red(p->get_right()) && p->get_right() && !is_red(p->get_right()->get_left())) {
         p = moveRedRight(p);
      }

      if (key == p->key) {
         rb_node *successor = getInOrderSuccessorNode(p);
         if (successor != nullptr) {
         p->value  = successor->value;  // Assign p in-order successor key and value
         p->key    = successor->key;
         }
         p->right = deleteMin(p->get_right()); // <-- deleteMin() appears to be where C++ delete should occur.

      } else {
         p->right = remove(p->get_right(), key);
      }
   }
   return fixUp(p);
}


/*
 * Returns key's associated value. The search for key starts in the subtree rooted at p.
 */
template<typename Key, typename Value>  Value rb_tree<Key, Value>::get(rb_node *p, Key key) throw(rb_exception)
{
   while (p != nullptr) {
       if      (key < p->key) p = p->get_left();
       else if (key > p->key) p = p->get_right();
       else             return p->value;
   }
   throw rb_exception();
}
/*
 * Returns in order successor of node p.
 */
template<typename Key, typename Value> inline
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::getInOrderSuccessorNode(rb_tree<Key, Value>::rb_node *p)
{
  p = p->get_right();
  while (p != nullptr) {
      p = p->get_left();
  }
  return p;
}

template<typename Key, typename Value>
typename rb_tree<Key, Value>::rb_node *rb_tree<Key, Value>::insert(rb_tree<Key, Value>::rb_node *p, Key key, Value value)
{
   if (p == nullptr)
      return new rb_node(key, value);
   // 2 3 4 tree implementation: do the equivalent of splitting a four node.
    if (is_red(p->get_left()) && is_red(p->get_right()))
        flip_color(p);
   if (key == p->key)
      p->value = value;
   else if (key < p->key)
      p->left = insert(p->get_left(), key, value);
   else
      p->right = insert(p->get_right(), key, value);
   if (is_red(p->get_right()))
      p = rotate_left(p);
   if (is_red(p->get_left()) && is_red(p->get_left()->get_left()))
      p = rotate_right(p);
   return p;
}

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, typename rb_tree<Key, Value>::rb_node &p) {
	os << p.key << " " << p.value;
	return os;
}

template<typename Key, typename Value> template<typename Functor> inline void rb_tree<Key, Value>::traverse(Functor f)
{
	::traverse_in_order<rb_tree<Key, Value>::rb_node, Functor>(f, root);
}

#endif
/* RB_TREE_H_ */
