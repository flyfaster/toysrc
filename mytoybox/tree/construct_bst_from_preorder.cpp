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

auto bst_from_preorder_inorder(vector<int> const& pre, int pre_pos,
                               vector<int> const& inorder, int start, int end)
{
    if (start > end)
        return std::make_tuple(static_cast<bst_node*>(nullptr), pre_pos);

    auto nd = new bst_node(pre[pre_pos++]);
    if (start == end)
        return std::make_tuple(nd, pre_pos);

    int mid = find(inorder.begin() + start,
    		inorder.begin() + end, nd->data) - inorder.begin();
    auto [lroot, next_pre_pos] =
        bst_from_preorder_inorder(pre, pre_pos, inorder, start, mid - 1);
    auto [rroot, next_pos] =
        bst_from_preorder_inorder(pre, next_pre_pos, inorder, mid + 1, end);
    nd->left = lroot;
    nd->right = rroot;
    return std::make_tuple(nd, next_pos);
}

int depth(bst_node* root)
{
    if (!root)
        return 0;
    return 1 + max(depth(root->left), depth(root->right));
}

BOOST_AUTO_TEST_CASE(check_bst_from_preorder_inorder)
{
    cout << "BOOST_AUTO_TEST_CASE(bst_from_preorder_inorder)" << endl;

    vector<int> in{4, 2, 5, 1, 6, 3};
    vector<int> pre{1, 2, 4, 5, 3, 6};
    auto [root, pos] = bst_from_preorder_inorder(pre, 0, in, 0, in.size() - 1);

    vector<int> out;
    inorder(root, out);

    BOOST_CHECK_EQUAL(1, root->data);
    BOOST_CHECK(equal(in.begin(), in.end(), out.begin()));

    BOOST_CHECK_EQUAL(3, depth(root));
}

auto bst_from_post_in(vector<int> const& post, int post_pos, vector<int> const& inorder,
                      int start, int end)
{
    if (start > end)
        return std::make_tuple(static_cast<bst_node*>(nullptr), post_pos);
    auto root = new bst_node(post[post_pos--]);
    if (start == end)
        return make_tuple(root, post_pos);

    auto mid = find(inorder.begin() + start, inorder.begin() + end, root->data) -
               inorder.begin();
    auto [rroot, rpos] = bst_from_post_in(post, post_pos, inorder, mid + 1, end);
    auto [lroot, lpos] = bst_from_post_in(post, rpos, inorder, start, mid - 1);
    root->left = lroot;
    root->right = rroot;
    return make_tuple(root, lpos);
}

void post_order_visit(bst_node* root, vector<int>& out)
{
    if (!root)
        return;
    post_order_visit(root->left, out);
    post_order_visit(root->right, out);
    out.push_back(root->data);
}

BOOST_AUTO_TEST_CASE(check_bst_from_post_in)
{
    cout << "BOOST_AUTO_TEST_CASE(bst_from_post_in)" << endl;

    vector<int> in{4, 8, 2, 5, 1, 6, 3, 7};
    vector<int> post{8, 4, 5, 2, 6, 7, 3, 1};
    auto [root, pos] = bst_from_post_in(post, post.size() - 1, in, 0, in.size() - 1);

    vector<int> out;
    post_order_visit(root, out);

    BOOST_CHECK_EQUAL(1, root->data);
    BOOST_CHECK_EQUAL(2, root->left->data);
    BOOST_CHECK_EQUAL(3, root->right->data);

    BOOST_CHECK(equal(post.begin(), post.end(), out.begin()));

    out.clear();
    inorder(root, out);
    BOOST_CHECK(equal(in.begin(), in.end(), out.begin()));

    BOOST_CHECK_EQUAL(4, depth(root));
}
