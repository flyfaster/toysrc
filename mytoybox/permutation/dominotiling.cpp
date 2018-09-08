#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/math/constants/constants.hpp>
using namespace std;

struct app_setup_t
{
    void parse_cmd_line(int argc, char* argv[])
    {
        // clang-format off
	    desc.add_options()
	    	("help,h", "Find number of ways to tile rows x cols grid")
			("rows", boost::program_options::value<int>(&rows)->default_value(rows),"specify value of rows")
	    	("cols", boost::program_options::value<int>(&cols)->default_value(cols),"specify value of columns")
			;
        // clang-format on
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }

    boost::program_options::variables_map vm;
    boost::program_options::options_description desc{"Allowed options"};
    int rows = 4;
    int cols = 7;
};

/*
let count(k, x) denote the number of ways to construct a solution for rows 1..k
of the grid such that string x corresponds to row k.
*/

class domino_tiling
{
    int m_height = 0;
    int m_width = 0;
    bool m_done = false;
    int m_tiling_count = 0;
    using str_vec_t = vector<string>;
    unordered_map<string, str_vec_t> m_cache;
    static constexpr char DOWN_TILE = 'd';
    static constexpr char UP_TILE = 'u';
    static constexpr char RIGHT_TILE = 'r';
    static constexpr char LEFT_TILE = 'l';

    str_vec_t first_row(int width)
    {
    	str_vec_t prev{"d", "r"};

        for (int col = 1; col < width; ++col)
        {
        	str_vec_t next;
        	for(auto iter = prev.rbegin(); iter!=prev.rend(); ++iter)
        	{
        		const string& candidate = *iter;
                switch (candidate.back())
                {
                case DOWN_TILE:
                case LEFT_TILE:
                    next.emplace_back(candidate + DOWN_TILE);
                    if (col != width - 1)
                        next.emplace_back(candidate + RIGHT_TILE);
                    break;
                case RIGHT_TILE:
                    next.emplace_back(candidate + LEFT_TILE);
                    break;
                default:
                	throw std::logic_error(string("unexpected data in ") + candidate);
                	break;
                }
        	}
            prev.swap(next);
        }

        return prev;
    }

    str_vec_t next_rows(const string& row)
    {
    	auto iter = m_cache.find(row);
        if (iter != m_cache.end())
            return iter->second;

        str_vec_t prev;

        if (row.front() == DOWN_TILE)
            prev.emplace_back("u");
        else
        {
            prev.emplace_back("d");
            if (row.length() > 1)
                prev.emplace_back("r");
        }

        for (size_t col = 1; col < row.length(); ++col)
        {
        	str_vec_t next;
        	for(auto iter = prev.rbegin(); iter!=prev.rend(); ++iter)
        	{
        		const string& candidate = *iter;
                switch (row[col])
                {
                case DOWN_TILE:
                    if (candidate[col - 1] != RIGHT_TILE)
                        next.emplace_back(candidate + UP_TILE);
                    break;
                case UP_TILE:
                case RIGHT_TILE:
                case LEFT_TILE:
                    if (candidate[col - 1] == RIGHT_TILE)
                        next.emplace_back(candidate + LEFT_TILE);
                    else
                    {
                        next.emplace_back(candidate + DOWN_TILE);
                        if (col != row.length() - 1)
                            next.emplace_back(candidate + RIGHT_TILE);
                    }
                }
        	}
            prev.swap(next);
        }
        m_cache[row] = prev;
        return prev;
    }

    int solve(int height, int width, const string& prev_row_layout, int cur_row)
    {
        if (cur_row == height)
        	return prev_row_layout.find(DOWN_TILE)==prev_row_layout.npos;

        auto rows = (cur_row?next_rows(prev_row_layout):first_row(width));
        int count = 0;
        for (const auto& row : rows)
            count += solve(height, width, row, cur_row + 1);

        return count;
    }

public:
    domino_tiling(int height, int width) : m_height(height), m_width(width)
    {
        if (height <= 0 || width <= 0 || height * width % 2)
            m_done = true;
    }

    int count()
    {
        if (!m_done)
        {
            m_tiling_count = solve(m_height, m_width, "", 0);
            m_done = true;
        }

        return m_tiling_count;
    }

    int tiling_formula()
    {
        const double pi = std::acos(-1); // boost::math::constants::pi<double>();
        double res = 1.0;
        for (int a = 1; a <= (m_height + 1) / 2; ++a)
            for (int b = 1; b <= (m_width + 1) / 2; ++b)
            {
                double c1 = std::cos((pi * a) / (double) (m_height + 1));
                double c2 = std::cos((pi * b) / (double) (m_width + 1));
                res *= 4 * (c1*c1 + c2*c2);
            }
        return (int) res;
    }
};

// https://codereview.stackexchange.com/questions/188300/counting-the-number-of-domino-tilings-for-a-rectangular-grid
int main(int argc, char* argv[]) {
	app_setup_t app;
	app.parse_cmd_line(argc, argv);
    if (app.vm.count("help"))
    {
        std::cout << app.desc << std::endl;
        return 0;
    }

    std::chrono::system_clock::time_point app_start_time =
        std::chrono::system_clock::now();
    domino_tiling dc(app.rows, app.cols);
    auto tiling_ways = dc.count();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - app_start_time);
    cout << "tiling_formula return " << dc.tiling_formula() << "\n";
    cout << "Program found " << tiling_ways << " ways in " << duration.count() << " milliseconds\n";
    return 0;
}
