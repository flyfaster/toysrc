#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

/*
Make-Set(x)
	x.p = x
	x.rank = 0

Union(x, y)
	link(find-set(x), find-set(y))

find-set(x)
	if x != x.p
		x.p = find-set(x.p)
	return x.p

link(x, y)
	if x.rank > y.rank
		y.p = x
	else
		x.p = y
		if x.rank = y.rank
			y.rank++
*/

class Solution
{
public:
    vector<int> circles;
    int get_root(int i)
    {
        if (circles[i] != i)
            circles[i] = get_root(circles[i]);
        return circles[i];
    }

    int findCircleNum(vector<vector<int>>& M)
    {
        for (int i = 0; i < M.size(); i++)
            circles.push_back(i);

        for (int i = 0; i < M.size(); ++i)
            for (int j = i + 1; j < M[0].size(); ++j)
            {
                if (!M[i][j])
                    continue;
                int lhs = get_root(i);
                int rhs = get_root(j);
                if (lhs != rhs)
                    circles[max(lhs, rhs)] = min(lhs, rhs);
            }

        int res = 0;
        for (int i = 0; i < circles.size(); ++i)
            if (circles[i] == i)
                ++res;
        return res;
    }
};

int main()
{
    vector<vector<int>> M{{1, 0, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 0, 1, 1}};
    cout << Solution().findCircleNum(M) << "\n";
}
