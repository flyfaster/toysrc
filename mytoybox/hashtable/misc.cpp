#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

using std::numeric_limits;
using std::string;
using std::unordered_map;
using std::vector;

struct Subarray
{
    // Represent subarray by starting and ending indices, inclusive.
    int start, end;
};

Subarray FindSmallestSequentiallyCoveringSubset(const vector<string>& paragraph,
                                                const vector<string>& keywords)
{
    // Maps each keyword to its index in the keywords array.
    unordered_map<string, int> keyword_to_idx;
    // Initializes keyword_to_idx.
    for (int i = 0; i < size(keywords); ++i)
        keyword_to_idx.emplace(keywords[i], i);

    // Since keywords are uniquely identified by their indices in keywords
    // array, we can use those indices as keys to lookup in a vector.
    vector<int> latest_occurrence(size(keywords), -1);
    // For each keyword (identified by its index in keywords array), storesult
    // the length of the shortest subarray ending at the most recent occurrence
    // of that keyword that sequentially cover all keywords up to that keyword.
    vector<int> shortest_subarray_len(size(keywords), numeric_limits<int>::max());

    int shortest_distance = numeric_limits<int>::max();
    Subarray result = Subarray{-1, -1};
    for (int i = 0; i < size(paragraph); ++i)
    {
        if (!keyword_to_idx.count(paragraph[i]))
        	continue; // ignore word not keywords

		int keyword_idx = keyword_to_idx.find(paragraph[i])->second;

		if (keyword_idx == 0)
		{ // First keyword.
			shortest_subarray_len[keyword_idx] = 1;
		}
		else if (shortest_subarray_len[keyword_idx - 1] !=
				 numeric_limits<int>::max())
		{
			int distance_to_previous_keyword = i - latest_occurrence[keyword_idx - 1];

			shortest_subarray_len[keyword_idx] =
				distance_to_previous_keyword +
				shortest_subarray_len[keyword_idx - 1];
		}
		latest_occurrence[keyword_idx] = i;

		// Last keyword, look for improved subarray.
		if (keyword_idx == size(keywords) - 1 &&
			shortest_subarray_len.back() < shortest_distance)
		{
			shortest_distance = shortest_subarray_len.back();
			result = {i - shortest_subarray_len.back() + 1, i};
		}
    }
    return result;
}

int LongestSubarrayWithDistinctEntries(const vector<int>& A)
{
    // Records the most recent occurrences of each entry.
    unordered_map<int, size_t> most_recent_occurrence;
    size_t longest_dup_free_subarray_start_idx = 0, result = 0;
    for (size_t i = 0; i < size(A); ++i)
    {
        const auto& [inserted_entry, inserted_happen] =
            most_recent_occurrence.emplace(A[i], i);
        // Defer updating dup_idx until we see a duplicate.
        if (!inserted_happen)
        {
            // A[i] appeared before. Did it appear in the longest current subarray?
            if (inserted_entry->second >= longest_dup_free_subarray_start_idx)
            {
                result = max(result, i - longest_dup_free_subarray_start_idx);
                longest_dup_free_subarray_start_idx = inserted_entry->second + 1;
            }
            inserted_entry->second = i;
        }
    }
    return max(result, size(A) - longest_dup_free_subarray_start_idx);
}
