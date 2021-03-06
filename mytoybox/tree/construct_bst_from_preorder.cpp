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

struct TreeNode
{
    int data{0};
    TreeNode* left{nullptr};
    TreeNode* right{nullptr};
    TreeNode(int d) : data{d}
    {
    }
};

TreeNode* construct_bst_preorder(vector<int> const& pre)
{
	if (pre.empty())
		return nullptr;

    stack<TreeNode*> st;
    TreeNode* root = new TreeNode(pre[0]);
    st.push(root);
    for (int i = 1; i < pre.size(); ++i)
    {
        TreeNode* temp = nullptr;
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
            temp->right = new TreeNode(pre[i]);
            st.push(temp->right);
        }
        // If the next value is less than the stack's top value, make this value
        // as the left child of the stack's top node. Push the new node to stack
        else
        {
            st.top()->left = new TreeNode(pre[i]);
            st.push(st.top()->left);
        }
    }
    return root;
}

void inorder(TreeNode* root, vector<int>& out)
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
    auto bst = construct_bst_preorder(pre);
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
        return std::make_tuple(static_cast<TreeNode*>(nullptr), pre_pos);

    auto root = new TreeNode(pre[pre_pos++]);
    if (start == end)
        return std::make_tuple(root, pre_pos);

    int mid = find(inorder.begin() + start, inorder.begin() + end, root->data) -
              inorder.begin();
    std::tie(root->left, pre_pos) =
        bst_from_preorder_inorder(pre, pre_pos, inorder, start, mid - 1);
    std::tie(root->right, pre_pos) =
        bst_from_preorder_inorder(pre, pre_pos, inorder, mid + 1, end);
    return std::make_tuple(root, pre_pos);
}

int depth(TreeNode* root)
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
        return std::make_tuple(static_cast<TreeNode*>(nullptr), post_pos);
    auto root = new TreeNode(post[post_pos--]);
    if (start == end)
        return make_tuple(root, post_pos);

    auto mid = find(inorder.begin() + start, inorder.begin() + end, root->data) -
               inorder.begin();

    std::tie(root->right, post_pos) =
        bst_from_post_in(post, post_pos, inorder, mid + 1, end);
    std::tie(root->left, post_pos) =
        bst_from_post_in(post, post_pos, inorder, start, mid - 1);
    return make_tuple(root, post_pos);
}

void post_order_visit(TreeNode* root, vector<int>& out)
{
    if (!root)
        return;
    post_order_visit(root->left, out);
    post_order_visit(root->right, out);
    out.push_back(root->data);
}

auto is_height_balanced(TreeNode* root)
{
    if (!root)
        return make_tuple(true, -1);
    auto [is_balance, left_height] = is_height_balanced(root->left);
    if (!is_balance)
        return make_tuple(is_balance, left_height);
    auto [is_right_balance, right_height] = is_height_balanced(root->right);
    if (!is_right_balance)
        return make_tuple(is_right_balance, right_height);
    if (abs(left_height - right_height) <= 1)
        return make_tuple(true, max(left_height, right_height) + 1);
    return make_tuple(false, max(left_height, right_height) + 1);
}

BOOST_AUTO_TEST_CASE(check_bst_from_post_in)
{
    cout << "BOOST_AUTO_TEST_CASE(bst_from_post_in)" << endl;

    vector<int> in{4, 8, 2, 5, 1, 6, 3, 7};
    vector<int> post{8, 4, 5, 2, 6, 7, 3, 1};
    auto [root, pos] = bst_from_post_in(post, post.size() - 1, in, 0, in.size() - 1);

    vector<int> out;
    post_order_visit(root, out);

    auto [balance, height] = is_height_balanced(root);
    BOOST_CHECK(balance);
    BOOST_CHECK_EQUAL((int)log2(in.size()), height);

    BOOST_CHECK_EQUAL(1, root->data);
    BOOST_CHECK_EQUAL(2, root->left->data);
    BOOST_CHECK_EQUAL(3, root->right->data);

    BOOST_CHECK(equal(post.begin(), post.end(), out.begin()));

    out.clear();
    inorder(root, out);
    BOOST_CHECK(equal(in.begin(), in.end(), out.begin()));

    BOOST_CHECK_EQUAL(4, depth(root));
}
