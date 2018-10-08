#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <list>
#include <tuple>
#include <memory>
#include <random>
#include <unordered_map>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE bst_rank
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

//#include <ext/pb_ds/assoc_container.hpp>
//#include <ext/pb_ds/tree_policy.hpp>

using namespace std;

struct RankNode
{
    int left_size = 0;
    RankNode *left = nullptr, *right = nullptr;
    int data = 0;
    RankNode(int d)
    {
        data = d;
    }

    void insert(int d)
    {
        if (d < data)
        {
            if (left)
                left->insert(d);
            else
                left = new RankNode(d);
            left_size++;
        }
        else
        {
            if (right)
                right->insert(d);
            else
                right = new RankNode(d);
        }
    }

    int order_of_key(int d)	// return -1 if d is not in the tree
    {
        if (d == data)
        {
            return left_size;
        }
        else if (d < data)
        {
            if (!left)
                return -1;
            else
                return left->order_of_key(d);
        }
        else
        {
            int right_rank = right == nullptr ? -1 : right->order_of_key(d);
            if (right_rank == -1)
                return -1;
            else
                return left_size + 1 + right_rank;
        }
    }

    int find_by_order(int rank)
    {
        if (rank == left_size)
            return data;
        if (rank < left_size)
            return left->find_by_order(rank);
        return right->find_by_order(rank - left_size - 1);
    }
};

BOOST_AUTO_TEST_CASE(check_sorted_list_to_bst)
{
    cout << "BOOST_AUTO_TEST_CASE(sorted_list_to_bst)" << endl;
    vector<int> Arr;
    random_device rd;
    std::uniform_int_distribution<int> dist(0, 65536);
    for (int i = 0; i < 128; i++)
        Arr.push_back(dist(rd));

    vector<int> sorted_arr = Arr;
    sort(sorted_arr.begin(), sorted_arr.end());

    auto iter = std::unique(sorted_arr.begin(), sorted_arr.end());
    sorted_arr.resize(std::distance(sorted_arr.begin(), iter));

    Arr = sorted_arr;
    std::mt19937 g(rd());

    std::shuffle(Arr.begin(), Arr.end(), g);

    unordered_map<int, int> rank;
    for (int i=0; i<sorted_arr.size(); ++i)
    	rank.emplace(sorted_arr[i], i);

//    using namespace __gnu_pbds;
//    using new_data_set =
//        tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update>;
//    new_data_set pbs_tree;

    RankNode* root = nullptr;
    for (int data: Arr)
    {
    	if (!root)
    	{
    		root = new RankNode(data);
    		continue;
    	}
    	else
    		root->insert(data);
//    	pbs_tree.insert(data);
    }

    for (int data: Arr)
    {
    	BOOST_CHECK_EQUAL(rank[data], root->order_of_key(data));
    }


    for (int data: sorted_arr)
    {
    	BOOST_CHECK_EQUAL(rank[data], root->order_of_key(data));
    }

    for (int rank = 0; rank < sorted_arr.size(); ++rank)
        BOOST_CHECK_EQUAL(sorted_arr[rank], root->find_by_order(rank));
}
