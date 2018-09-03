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
#include <iterator>

using namespace std;
//https://en.wikipedia.org/wiki/Reservoir_sampling
vector<int> sample_k(vector<int> const& stream, int k)
{
    vector<int> reservoir(k);
    copy(stream.begin(), stream.begin() + k, reservoir.begin());
    srand(time(nullptr));

    // Iterate from the (k+1)th element to nth element
    for (int i = k; i < stream.size(); i++)
    {
        // Pick a random index from 0 to i.
        // If the randomly  picked index is smaller than k, then replace
        // the element present at the index with new element from stream
        int random_val = rand() % (i + 1);
        if (random_val < k)
            reservoir[random_val] = stream[i];
    }
    return reservoir;
}

int sample_one(vector<int> const& stream)
{
	srand(time(nullptr));
    int res = stream[0];
    for (int count = 1; count < stream.size(); ++count)
    {
        // Replace the random number with new number with 1/count+1 probability
        if ((rand() % (count + 1)) == count)
            res = stream[count];
    }
    return res;
}

int main()
{
	cout << "Reservoir_sampling\n";
	vector<int> stream{1, 4, 8, 2, 5, 7, 9, 0, 6};
	cout << sample_one(stream) << "\n";
	vector<int> res = sample_k(stream, stream.size()/2);
	copy(res.begin(), res.end(), ostream_iterator<int>(cout, " "));
	cout << "\n";
	return 0;
}
