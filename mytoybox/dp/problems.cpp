#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
#include <numeric>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

vector<int> w{1,3,3,5};
// possible(x,k) = possible(x-w[k], k-1) | possible(x, k-1)

bool knapsack(vector<int> const& w,
		int n, // number of items to use
		int W) // total weights
{
	vector<vector<int>> possible(W+1, vector<int>(n+1, 0));	// IOI
	for (int i=0; i<possible.size(); ++i)
		possible[i][0] = true;

    for (int k = 1; k <= n; k++)
        for (int x = 0; x <= W; x++)
        {
        	possible[x][k] |= possible[x][k - 1];
            if (x >= w[k])
                possible[x][k] |= possible[x - w[k]][k - 1];
        }
    return possible[W][n];
}

bool knapsack(vector<int> const& w, int capacity)
{
    vector<int> possible(capacity+1, 0);	// IOI
    possible[0] = true;
    for (int k = 1; k <= w.size(); k++)
        for (int x = capacity; x >= 0; x--)
        {
        	auto item_size = w[k];

            if (x + item_size < possible.size() && possible[x])
                possible[x + item_size] = true;
        }
    return possible[capacity];
}

// can partition set to two sets with equal sum?
bool subset_sum(vector<int> nums, int sum)
{
    deque<bool> dp(sum + 1, 0);
    dp[0] = true;

    for (int num : nums)
    {
        for (int i = sum; i > 0; i--)
        {
            if (i >= num)
            {
                dp[i] = dp[i] || dp[i - num];
            }
        }
    }

    return dp[sum];
}

struct Item
{
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
    if (V[items_rbound][available_capacity] == -1) // not processed yet
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

int MinimumMessiness(const vector<string>& words, int line_length)
{	// sentence screen fitting, line breaking algorithm, word wrapping
    // minimum_messiness[i] is the minimum messiness when placing words[0, i].
    vector<int> minimum_messiness(size(words), numeric_limits<int>::max());
    int num_remaining_blanks = line_length - size(words[0]);
    minimum_messiness[0] = num_remaining_blanks * num_remaining_blanks;
    for (int i = 1; i < size(words); ++i)
    {
        num_remaining_blanks = line_length - size(words[i]);
        minimum_messiness[i] =
            minimum_messiness[i - 1] + num_remaining_blanks * num_remaining_blanks;
        // Try adding words[i - 1], words[i - 2], ...
        for (int j = i - 1; j >= 0; --j)
        {
            num_remaining_blanks -= (size(words[j]) + 1);
            if (num_remaining_blanks < 0)
            {
                // Not enough space to add more words.
                break;
            }
            int first_j_messiness = j - 1 < 0 ? 0 : minimum_messiness[j - 1];
            int current_line_messiness = num_remaining_blanks * num_remaining_blanks;
            minimum_messiness[i] =
                min(minimum_messiness[i], first_j_messiness + current_line_messiness);
        }
    }
    return minimum_messiness.back();
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

/* Returns length of longest common substring of X[0..m-1]
   and Y[0..n-1] */
int LCSubStr(char *X, char *Y, int m, int n)
{
    // Create a table to store lengths of longest common suffixes of
    // substrings.   Note that LCSuff[i][j] contains length of longest
    // common suffix of X[0..i-1] and Y[0..j-1]. The first row and
    // first column entries have no logical meaning, they are used only
    // for simplicity of program
	vector<vector<int>> LCSuff(m+1, vector<int>(n+1, 0));
    int result = 0;  // To store length of the longest common substring

    /* Following steps build LCSuff[m+1][n+1] in bottom up fashion. */
    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
                LCSuff[i][j] = 0;

            else if (X[i - 1] == Y[j - 1])
            {
                LCSuff[i][j] = LCSuff[i - 1][j - 1] + 1;
                result = max(result, LCSuff[i][j]);
            }
            else
                LCSuff[i][j] = 0;
        }
    }

    return result;
}
