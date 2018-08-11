#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <set>
#include <chrono>
using namespace std;

void permutation1(vector<int>& nums, int n, vector<vector<int>>& res)
{
    if (n == 1)
    {
        res.emplace_back(nums);
        return;
    }
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
    {
        res.emplace_back(vector<int>{nums[n - 1]});
        return;
    }
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
    {
        res.emplace_back(nums);
        return;
    }
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

template <typename PermutationFunc>
void check_permutation_algorithm(const char* alg_name, PermutationFunc alg)
{
    vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    set<vector<int>> sv;
    vector<vector<int>> res;
    std::chrono::system_clock::time_point app_start_time =
        std::chrono::system_clock::now();
    permutation1(nums, nums.size(), res);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - app_start_time);
    for (auto& vec : res)
        sv.insert(vec);
    cout << alg_name << " generated " << res.size() << " rows in " << duration.count()
         << " milliseconds, there are " << sv.size() << " unique permutations\n";
}

int main()
{
    check_permutation_algorithm("Remove method", permutation1);
    check_permutation_algorithm("Insert method", permutation2);
    check_permutation_algorithm("heaps_permutation", heaps_permutation);
    return 0;
}

//Remove method generated 3628800 rows in 793 milliseconds, there are 3628800 unique permutations
//Insert method generated 3628800 rows in 714 milliseconds, there are 3628800 unique permutations
//heaps_permutation generated 3628800 rows in 707 milliseconds, there are 3628800 unique permutations
