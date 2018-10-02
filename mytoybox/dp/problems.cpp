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

struct Item {
  int weight, value;
};

// Returns the optimum value when we choose from items[0, k] and have a
// capacity of available_capacity.
int OptimumSubjectToItemAndCapacity(const vector<Item>& items, int items_rbound,
                                    int available_capacity, vector<vector<int>>& V)
{
    if (items_rbound < 0) // No items can be chosen.
        return 0;

    // V[i][j] holds the optimum value when we choose from items[0, i] and have
    // a capacity of j.
    if (V[items_rbound][available_capacity] == -1)
    {
        int without_curr_item = OptimumSubjectToItemAndCapacity(items, items_rbound - 1,
                                                                available_capacity, V);
        int with_curr_item =
            available_capacity < items[items_rbound].weight ?
                0 :
                items[items_rbound].value +
                    OptimumSubjectToItemAndCapacity(
                        items, items_rbound - 1,
                        available_capacity - items[items_rbound].weight, V);
        V[items_rbound][available_capacity] = max(without_curr_item, with_curr_item);
    }
    return V[items_rbound][available_capacity];
}

int OptimumSubjectToCapacity(const vector<Item>& items, int capacity)
{
    vector<vector<int>>> dp(size(items), vector<int>(capacity + 1, -1));

    return OptimumSubjectToItemAndCapacity(items, size(items) - 1, capacity, dp);
}

vector<string> DecomposeIntoDictionaryWords(const string& domain,
                                            const unordered_set<string>& dictionary)
{
    // When the algorithm finishes, last_length[i] != -1 indicates
    // domain.substr(0, i + 1) has a valid decomposition, and the length of the
    // last string in the decomposition is last_length[i].
    vector<int> last_length(size(domain), -1);
    for (int i = 0; i < size(domain); ++i)
    {
        // If domain.substr(0, i + 1) is a dictionary word, set last_length[i] to
        // the length of that word.
        if (dictionary.count(domain.substr(0, i + 1)))
        {
            last_length[i] = i + 1;
            continue;
        }

        // If domain.substr(0, i + 1) is not a dictionary word, we look for j < i
        // such that domain.substr(0, j + 1) has a valid decomposition and
        // domain.substring(j + 1, i + 1) is a dictionary word. If so, record the
        // length of that word in last_length[i].
        for (int j = 0; j < i; ++j)
        {
            if (last_length[j] != -1 && dictionary.count(domain.substr(j + 1, i - j)))
            {
                last_length[i] = i - j;
                break;
            }
        }
    }

    vector<string> decompositions;
    if (last_length.back() != -1)
    {
        // domain can be assembled by dictionary words.
        int idx = size(domain) - 1;
        while (idx >= 0)
        {
            decompositions.emplace_back(
                domain.substr(idx + 1 - last_length[idx], last_length[idx]));
            idx -= last_length[idx];
        }
        reverse(begin(decompositions), end(decompositions));
    }
    return decompositions;
}

int MinimumPathWeight(const vector<vector<int>>& triangle)
{
    if (empty(triangle))
        return 0;

    // As we iterate, prev_row stores the minimum path sum to each entry in
    // triangle[i - 1].
    vector<int> prev_row(triangle.front());
    for (int i = 1; i < size(triangle); ++i)
    {
        // Stores the minimum path sum to each entry in triangle[i].
        vector<int> curr_row(triangle[i]);
        curr_row.front() += prev_row.front(); // For the first element.
        for (int j = 1; j < size(curr_row) - 1; ++j)
        {
            curr_row[j] += min(prev_row[j - 1], prev_row[j]);
        }

        curr_row.back() += prev_row.back(); // For the last element.

        // Uses swap to assign curr_row's content to prev_row in O(1) time.
        prev_row.swap(curr_row);
    }
    return *min_element(cbegin(prev_row), cend(prev_row));
}

bool MatchAllWordsInDict(const string& s, const unordered_map<string, int>& word_to_freq,
                         int start, int num_words, int unit_size)
{
    unordered_map<string, int> curr_string_to_freq;
    for (int i = 0; i < num_words; ++i)
    {
        string curr_word = s.substr(start + i * unit_size, unit_size);
        if (auto iter = word_to_freq.find(curr_word); iter == end(word_to_freq))
        {
            return false;
        }
        else
        {
            ++curr_string_to_freq[curr_word];
            if (curr_string_to_freq[curr_word] > iter->second)
            {
                // curr_word occurs too many times for a match to be possible.
                return false;
            }
        }
    }
    return true;
}

vector<int> FindAllSubstrings(const string& s, const vector<string>& words)
{
    unordered_map<string, int> word_to_freq;
    for (const string& word : words)
    {
        ++word_to_freq[word];
    }

    int unit_size = size(words.front());
    vector<int> result;
    for (int i = 0; i + unit_size * size(words) <= size(s); ++i)
    {
        if (MatchAllWordsInDict(s, word_to_freq, i, size(words), unit_size))
        {
            result.emplace_back(i);
        }
    }
    return result;
}
