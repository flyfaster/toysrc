/*
	Author: Kevin Bacon
	Date: 04/03/2014
	Task: Closest-pair problem
*/

//	http://rosettacode.org/wiki/Closest-pair_problem#C.2B.2B
//	CLRS 33.4 Finding the closest pair of points
//	https://people.csail.mit.edu/indyk/6.838-old/handouts/lec17.pdf
//	Closest Pair in Higher Dimensions

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <iterator>

using point_t = std::pair<double, double> ;
using points_t = std::pair<point_t, point_t> ;

double distance_between(const point_t& a, const point_t& b)
{
    return std::hypot(b.first - a.first, b.second - a.second);
}

std::pair<double, points_t> find_closest_brute(const std::vector<point_t>& points)
{
    if (points.size() < 2)
        return {-1, {{0, 0}, {0, 0}}};

    auto minDistance = std::abs(distance_between(points.at(0), points.at(1)));
    points_t minPoints = {points.at(0), points.at(1)};
    for (auto i = std::begin(points); i != (std::end(points) - 1); ++i)
    {
        for (auto j = i + 1; j < std::end(points); ++j)
        {
            auto newDistance = std::abs(distance_between(*i, *j));
            if (newDistance < minDistance)
            {
                minDistance = newDistance;
                minPoints.first = *i;
                minPoints.second = *j;
            }
        }
    }
    return {minDistance, minPoints};
}

std::pair<double, points_t> find_closest_optimized(const std::vector<point_t>& xP,
                                                   const std::vector<point_t>& yP)
{
    if (xP.size() <= 3)
        return find_closest_brute(xP);

    auto N = xP.size();
    auto xL = std::vector<point_t>();
    auto xR = std::vector<point_t>();
    std::copy(std::begin(xP), std::begin(xP) + (N / 2), std::back_inserter(xL));
    std::copy(std::begin(xP) + (N / 2), std::end(xP), std::back_inserter(xR));
    auto mid_x = xP.at((N - 1) / 2).first;
    auto yL = std::vector<point_t>();
    auto yR = std::vector<point_t>();
    std::copy_if(std::begin(yP), std::end(yP), std::back_inserter(yL),
                 [&mid_x](const point_t& p) { return p.first <= mid_x; });
    std::copy_if(std::begin(yP), std::end(yP), std::back_inserter(yR),
                 [&mid_x](const point_t& p) { return p.first > mid_x; });
    auto p1 = find_closest_optimized(xL, yL);
    auto p2 = find_closest_optimized(xR, yR);
    auto minPair = (p1.first <= p2.first) ? p1 : p2;
    auto yS = std::vector<point_t>();
    std::copy_if(std::begin(yP), std::end(yP), std::back_inserter(yS),
                 [&minPair, &mid_x](const point_t& p) {
                     return std::abs(mid_x - p.first) < minPair.first;
                 });
    auto result = minPair;
    for (auto i = std::begin(yS); i != (std::end(yS) - 1); ++i)
    {
        for (auto k = i + 1;
             k != std::end(yS) && ((k->second - i->second) < minPair.first); ++k)
        {
            auto newDistance = std::abs(distance_between(*k, *i));
            if (newDistance < result.first)
            {
                result = {newDistance, {*k, *i}};
            }
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& ostrm, const point_t& point)
{
    ostrm << "(" << point.first << ", " << point.second << ")";
    return ostrm;
}

int main(int argc, char* argv[])
{
    std::default_random_engine re(
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    std::uniform_real_distribution<double> urd(-500.0, 500.0);
    std::vector<point_t> points(100);
    std::generate(std::begin(points), std::end(points), [&urd, &re]() {
        return point_t{1000 + urd(re), 1000 + urd(re)};
    });
    auto answer = find_closest_brute(points);
    std::sort(std::begin(points), std::end(points),
              [](const point_t& a, const point_t& b) { return a.first < b.first; });
    auto xP = points;
    std::sort(std::begin(points), std::end(points),
              [](const point_t& a, const point_t& b) { return a.second < b.second; });
    auto yP = points;
    std::cout << "Min distance (brute): " << answer.first << " " << answer.second.first;
    std::cout << ", " << answer.second.second << "\n";

    answer = find_closest_optimized(xP, yP);
    std::cout << "Min distance (optimized): " << answer.first << " "
              << answer.second.first;
    std::cout << ", " << answer.second.second << "\n";

    return 0;
}
