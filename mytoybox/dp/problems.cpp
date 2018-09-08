#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>

// knapsack problem
using namespace std;

vector<int> w{1,3,3,5};
// possible(x,k) = possible(x-w[k], k-1) | possible(x, k-1)

void knapsack(vector<int> const& w,
		int n, // number of items to use
		int W) // total weights
{
	vector<vector<int>> possible(W, vector<int>(n));
	possible[0][0] = true;
    for (int k = 1; k <= n; k++)
        for (int x = 0; x <= W; x++)
            if (x >= w[k])
                possible[x][k] |= possible[x - w[k]][k - 1];
    possible[x][k] |= possible[x][k - 1];
}

void knapsack(vector<int> const& w, int W)
{
    vector<int> possible(W, false);
    possible[0] = true;
    for (int k = 1; k <= w.size(); k++)
        for (int x = W; x >= 0; x--)
            if (x < possible.size() && possible[x])
                possible[x + w[k]] = true;
}
