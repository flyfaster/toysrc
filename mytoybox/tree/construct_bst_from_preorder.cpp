#include <stack>
#include <limits>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE construct_bst_from_preorder
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>
using namespace std;

struct bst_node
{
    int data{0};
    bst_node* left{nullptr};
    bst_node* right{nullptr};
    bst_node(int d) : data{d}
    {
    }
};

bst_node* construct_bst(vector<int> const& pre)
{
	if (pre.empty())
		return nullptr;
    stack<bst_node*> st;
    bst_node* root = new bst_node(pre[0]);
    st.push(root);
    for (int i = 1; i < pre.size(); ++i)
    {
        bst_node* temp = nullptr;
        /* Keep on popping while the next value is greater than
           stack's top value. */
        while (!st.empty() && pre[i] > st.top()->data)
        {
            temp = st.top();
            st.pop();
        }
        // Make this greater value as the right child and push it to the stack
        if (temp != nullptr)
        {
            temp->right = new bst_node(pre[i]);
            st.push(temp->right);
        }
        // If the next value is less than the stack's top value, make this value
        // as the left child of the stack's top node. Push the new node to stack
        else
        {
            st.top()->left = new bst_node(pre[i]);
            st.push(st.top()->left);
        }
    }
    return root;
}

void inorder(bst_node* root, vector<int>& out)
{
    if (!root)
        return;
    inorder(root->left, out);
    out.push_back(root->data);
    inorder(root->right, out);
}

BOOST_AUTO_TEST_CASE(check_construct_bst_from_preorder)
{
    cout << "BOOST_AUTO_TEST_CASE(construct_bst_from_preorder)" << endl;
    int arr[] = {9, 5, 3, 7, 40, 99};
    int size = std::extent<decltype(arr)>::value;
    vector<int> pre;
    pre.assign(begin(arr), end(arr));
    auto bst = construct_bst(pre);
    vector<int> out;
    inorder(bst, out);

    for (int i = 0; i < size - 1; ++i)
        BOOST_CHECK(out[i] < out[i + 1]);

    BOOST_CHECK_EQUAL(3, out[0]);
    BOOST_CHECK_EQUAL(99, out[size - 1]);
    sort(begin(arr), end(arr));
    for (int i = 0; i < size; ++i)
    	BOOST_CHECK_EQUAL(out[i], arr[i]);
}
