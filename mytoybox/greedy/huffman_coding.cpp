#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE huffman_code
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

struct huffman_node
{
    char data{0};
    // Frequency of the character
    unsigned freq{0};
    huffman_node* left{nullptr};
    huffman_node* right{nullptr};
    huffman_node(char ac, unsigned frequency) : data(ac), freq{frequency}
    {
    }
    // create internal node
    huffman_node(huffman_node* lc, huffman_node* rc) :
        data(internal_node),
        left{lc},
        right{rc},
        freq{lc->freq + rc->freq}
    {
    }
    bool is_internal()
    {
        return data == internal_node;
    }
    static constexpr char internal_node = '$';
};

struct min_heap_cmp
{
    bool operator()(const huffman_node* l, const huffman_node* r) const
    {
        return (l->freq > r->freq);
    }
};

void heap_to_codes(struct huffman_node* root, string str, unordered_map<char, string>& hc)
{
    if (!root)
        return;
    if (!root->is_internal()) {
    	hc[root->data] = str;
    	return;
    }
    heap_to_codes(root->left, str + "0", hc);
    heap_to_codes(root->right, str + "1", hc);
}

unordered_map<char, string> huffman_coder(const char data[], const int freq[], int size)
{
    priority_queue<huffman_node*, vector<huffman_node*>, min_heap_cmp> min_heap;

    for (int i = 0; i < size; ++i)
        min_heap.emplace(new huffman_node(data[i], freq[i]));

    while (min_heap.size() > 1)
    {
        auto left = min_heap.top();
        min_heap.pop();

        auto right = min_heap.top();
        min_heap.pop();

        min_heap.emplace(new huffman_node(left, right));
    }

    unordered_map<char, string> hc;
    heap_to_codes(min_heap.top(), "", hc);
    return hc;
}

BOOST_AUTO_TEST_CASE(check_cut_rod_recursive)
{
    cout << "BOOST_AUTO_TEST_CASE(huffman_coder)" << endl;
    char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    int freq[] = { 6, 9, 12, 13, 16, 55 };

    int size = std::extent<decltype(arr)>::value;

    auto hc = huffman_coder(arr, freq, size);

    BOOST_TEST_CONTEXT("huffman_coder(f)")
    BOOST_CHECK_EQUAL("0", hc['f']);
    BOOST_TEST_CONTEXT("huffman_coder(c)")
    BOOST_CHECK_EQUAL("100", hc['c']);
    BOOST_TEST_CONTEXT("huffman_coder(d)")
    BOOST_CHECK_EQUAL("101", hc['d']);
    BOOST_TEST_CONTEXT("huffman_coder(a)")
    BOOST_CHECK_EQUAL("1100", hc['a']);
    BOOST_TEST_CONTEXT("huffman_coder(b)")
    BOOST_CHECK_EQUAL("1101", hc['b']);
    BOOST_CHECK_EQUAL("111", hc['e']);
}
