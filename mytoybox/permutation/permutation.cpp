#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <numeric>
#include <set>
#include <unordered_set>
#include <chrono>
#include <boost/functional/hash.hpp>
using namespace std;

void permutation1(vector<int>& nums, int n, vector<vector<int>>& res)
{
    if (n == 1)
    	return res.emplace_back(nums);

    for (int i = 0; i < n; ++i)
    {
        swap(nums[i], nums[n - 1]);
        permutation1(nums, n - 1, res);
        swap(nums[i], nums[n - 1]);
    }
}

void permutation2(vector<int>& nums, int n, vector<vector<int>>& res)
{
    if (n == 1)
        return res.emplace_back(vector<int>{nums[n - 1]});

    vector<vector<int>> res2;
    permutation2(nums, n - 1, res2);
    for (const auto& vec : res2)
    {
        for (int i = 0; i <= vec.size(); ++i)
        {
            vector<int> row;
            copy(vec.begin(), vec.begin() + i, back_inserter(row));
            row.push_back(nums[n - 1]);
            if (i < vec.size())
                copy(vec.begin() + i, vec.end(), back_inserter(row));
            res.emplace_back(move(row));
        }
    }
}

void heaps_permutation(vector<int>& nums, int n, vector<vector<int>>& res)
{
    if (n == 1)
        return res.emplace_back(nums);

    for (int i = 0; i < n - 1; ++i)
    {
        heaps_permutation(nums, n - 1, res);
        if (n % 2 == 0)
            swap(nums[n - 1], nums[i]);
        else
            swap(nums[n - 1], nums[0]);
    }
    heaps_permutation(nums, n - 1, res);
}

void search(vector<int>& permutation, int n, vector<int>& chosen, vector<vector<int>>& res) {
	if (permutation.size() == n) {
		res.emplace_back(permutation);
	} else {
		for (int i = 0; i < n; i++) {
			if (chosen[i])
				continue;
			chosen[i] = true;
			permutation.push_back(i);
			search(permutation, n, chosen, res);
			chosen[i] = false;
			permutation.pop_back();
		}
	}
}

void search_permutation(vector<int>& permutation, int n, vector<vector<int>>& res) {
	vector<int> chosen(permutation.size(), 0);
	search(permutation, n, chosen, res);
}

void stl_permutation(vector<int>& permutation, int n, vector<vector<int>>& res) {
	permutation.resize(n);
	std::iota(permutation.begin(), permutation.end(), 1);
	do {
		res.emplace_back(permutation);
	} while (next_permutation(permutation.begin(), permutation.end()));
}

struct hash_vec
{
    size_t operator()(vector<int> const& s) const
    {
        return boost::hash_range(s.begin(), s.end());
    }
};

template <typename PermutationFunc>
void check_complete_search_algorithm(const char* alg_name, PermutationFunc alg)
{
    vector<int> nums(9);
    iota(nums.begin(), nums.end(), 1);

    vector<vector<int>> res;
    std::chrono::system_clock::time_point app_start_time =
        std::chrono::system_clock::now();
    permutation1(nums, nums.size(), res);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - app_start_time);

    unordered_set<vector<int>, hash_vec> sv(res.size());
    for (auto& vec : res)
        sv.emplace(vec);
    cout << alg_name << " generated " << res.size() << " rows in " << duration.count()
         << " milliseconds, there are " << sv.size() << " unique permutations\n";
}

void gen_subset(vector<int>& nums, int n, vector<vector<int>>& res) {
	vector<int> subset;
	std::function<void(int)> search;
	search = [&nums, &subset, &res, &search](int k) {
		if (k==nums.size())
			return res.emplace_back(subset);

		search(k+1);
		subset.push_back(nums[k]);
		search(k+1);
		subset.pop_back();
	};
	search(0);
}

void gen_subset2(vector<int>& nums, int n, vector<vector<int>>& res) {
	for (int i=0; i< (1<<n); ++i) {
		bitset<numeric_limits<int>::digits> bits(i);
		vector<int> subset;
		for (int bit=0; bit<n; ++bit) {
			if (bits[bit])
				subset.push_back(nums[bit]);
		}
		res.emplace_back(move(subset));
	}
}

int main()
{
    std::chrono::system_clock::time_point app_start_time =
        std::chrono::system_clock::now();
    check_complete_search_algorithm("Remove method", permutation1);
    check_complete_search_algorithm("Insert method", permutation2);
    check_complete_search_algorithm("heaps_permutation", heaps_permutation);
    check_complete_search_algorithm("search_permutation", search_permutation);
    check_complete_search_algorithm("stl_permutation", stl_permutation);
    check_complete_search_algorithm("gen_subset", gen_subset);
    check_complete_search_algorithm("gen_subset2", gen_subset2);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - app_start_time);
    cout << "Program done in " << duration.count() << " milliseconds\n";
    return 0;
}

//Remove method generated 3628800 rows in 793 milliseconds, there are 3628800 unique permutations
//Insert method generated 3628800 rows in 714 milliseconds, there are 3628800 unique permutations
//heaps_permutation generated 3628800 rows in 707 milliseconds, there are 3628800 unique permutations
