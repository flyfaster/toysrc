#include <iostream>
#include <cstddef>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE greedy
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

// gallons[i] is the amount of gas in city i, and distances[i] is the distance
// city i to the next city.
int FindAmpleCity(const vector<int>& gallons, const vector<int>& distances)
{
    int cur_gallons = 0;
    struct CityAndRemainingGas
    {
        int city = 0, remaining_gallons = 0;
    };
    CityAndRemainingGas city_remaining_gallons;

    for (int i = 1; i < gallons.size(); ++i)
    {
        static constexpr int kMPG = 20;
        cur_gallons += gallons[i - 1] - distances[i - 1] / kMPG;

        if (cur_gallons < city_remaining_gallons.remaining_gallons)
            city_remaining_gallons = {i, cur_gallons};
    }
    cout << "remaining_gallons = " << city_remaining_gallons.remaining_gallons << "\n";
    return city_remaining_gallons.city;
}

BOOST_AUTO_TEST_CASE(check_gas_up)
{
    cout << "BOOST_AUTO_TEST_CASE(gas up)" << endl;
    vector<int> gallons{50, 20, 5, 30, 25, 10, 10};
    vector<int> distances{900, 600, 200, 400, 600, 200, 100};

    auto city = FindAmpleCity(gallons, distances);
    BOOST_TEST_CONTEXT("city 3 is ample sity (start city with empty tank)")
    BOOST_CHECK_EQUAL(3, city);
}

bool IsNewPillarOrReachEnd(const vector<int>& heights, int curr_idx, int last_pillar_idx)
{
    return curr_idx < size(heights) ? heights[curr_idx] <= heights[last_pillar_idx] :
                                      true;
}

int CalculateLargestRectangle(const vector<int>& heights)
{
    stack<int> pillar_indices;
    int max_rectangle_area = 0;
    // By iterating to size(heights) instead of size(heights) - 1, we can
    // uniformly handle the computation for rectangle area here.
    for (int i = 0; i <= size(heights); ++i)
    {
        while (!empty(pillar_indices) &&
               IsNewPillarOrReachEnd(heights, i, pillar_indices.top()))
        {
            int height = heights[pillar_indices.top()];
            pillar_indices.pop();
            int width = empty(pillar_indices) ? i : i - pillar_indices.top() - 1;
            max_rectangle_area = max(max_rectangle_area, height * width);
        }
        pillar_indices.emplace(i);
    }
    return max_rectangle_area;
}
