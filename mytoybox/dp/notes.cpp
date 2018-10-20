#include <limits>
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;
const char* optimal_bst=R"(
e[i][j] the expected cost of searching an optimal binary search tree containing the keys k[i..j].
w[i][j] the sum of all the probabilities in the subtree k[i..j]
e[i][j] = e[i][r-1] + e[r+1][j] + w[i][j];

optimal_bst(float* p, // p[i] is probability of key[i]
	float* q, // q[i] is probability of non-existing dummy keys
	n)
{
	float e[1+n][n+1];
	float w[n+1][n+1];
	int root[n][n]; // root of keys[i..j]
	for (int i=1; i<n+1; ++i)
	{
		e[i][i-1] = q[i-1];
		w[i][i-1] = q[i-1];
	}
	for (int l=1; l<n; ++l)
		for (int i=1; i<n-l+1; ++i)
		{
			int j = i+l-1;
			e[i][j] = INF;
			w[i][j] = w[i][j-1] + p[j] + q[j];
			for (int r=i; r<j; ++r)
			{
				auto t = e[i][r-1] + e[r+1][j] + w[i][j];
				if (t<e[i][j])
				{
					e[i][j] = t;
					root[i][j] = r;
				}
			}
		}
}

// https://walkccc.github.io/CLRS/Chap15/15.5/
knuth_optimal_bst(float* p, // p[i] is probability of key[i]
	float* q, // q[i] is probability of non-existing dummy keys
	n)
{
	float e[1+n][n+1];
	float w[n+1][n+1];
	int root[n][n]; // root of keys[i..j]
	for (int i=1; i<n+1; ++i)
	{
		e[i][i-1] = q[i-1];
		w[i][i-1] = q[i-1];
	}
	for (int l=1; l<n; ++l)
		for (int i=1; i<n-l+1; ++i)
		{
			int j = i+l-1;
			e[i][j] = INF;
			w[i][j] = w[i][j-1] + p[j] + q[j];
			for (int r=root[i][ j - 1]; r<root[i + 1][ j]; ++r)
			{
				auto t = e[i][r-1] + e[r+1][j] + w[i][j];
				if (t<e[i][j])
				{
					e[i][j] = t;
					root[i][j] = r;
				}
			}
		}
}

)";

// https://www.cise.ufl.edu/~sahni/ca/Pgm5.5
const int SIZE=20;
int Find(float c[][SIZE],	// cost
		int r[][SIZE], 		// roots
		int i, int j)
{
    float min = FLT_MAX;
    int l;	// best root in range r[i][j-1] <= l <= r[i+1][j]
    for (int m = r[i][j - 1]; m <= r[i + 1][j]; m++)
        if ((c[i][m - 1] + c[m][j]) < min)
        {
            min = c[i][m - 1] + c[m][j];
            l = m;
        }
    return (l);
}

void OBST(float p[], // probability of key exists
		float q[], 	 // probability of keys not in the tree
		int n)
    // Given n distinct identifiers a_1<a_2<...<a_n
    // and probabilities p[i], 1<=i<=n, and q[i], 0<=i<=n,
    // this algorithm computes the cost c[i][j] of
    // optimal binary search trees t_{i..j} for identifiers
    // a_{i+1},...,a_j. It also computes r[i][j], the
    // root of t_{i..j}. w[i][j] is the weight of t_{i..j}.
{
    float cost[SIZE][SIZE]={}; // cost, initialize all to 0.0
    float w[SIZE][SIZE]={};
    int root[SIZE][SIZE]={};	// roots, initialize all to 0
    for (int i = 0; i < n; i++)
    {
        // Initialize.
        w[i][i] = q[i];
        // Optimal trees with one node
        w[i][i + 1] = q[i] + q[i + 1] + p[i + 1];
        root[i][i + 1] = i + 1;
        cost[i][i + 1] = q[i] + q[i + 1] + p[i + 1];
    }
    w[n][n] = q[n];
    root[n][n] = 0;
    cost[n][n] = 0.0;
    for (int m = 2; m <= n; m++) // Find optimal trees
                                 // with m nodes.
        for (int i = 0; i <= n - m; i++)
        {
            int j = i + m;
            w[i][j] = w[i][j - 1] + p[j] + q[j];
            // Solve 5.12 using Knuth's result.
            int k = Find(cost, root, i, j);
            // A value of l in the range
            // r[i][j-1] <= l <= r[i+1][j] that
            // minimizes {c[i][l-1]+c[l][j]};
            cost[i][j] = w[i][j] + cost[i][k - 1] + cost[k][j];
            root[i][j] = k;
        }
    std::cout << cost[0][n] << ' ' << w[0][n] << ' ' << root[0][n];
}
