#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
// refer to https://rosettacode.org/wiki/A*_search_algorithm#C.2B.2B

const char* A_star=R"(
// A* Search Algorithm
g = the movement cost to move from the starting point to a given cell on the grid, following the path generated to get there.
h = the estimated movement cost to move from that given cell on the grid to the final destination.
f = g + h

1.  Initialize the open list, put the starting node on the open list (you can leave its f at zero)
2.  Initialize the closed list
3.  while the open list is not empty
    a) find the node with the least f on the open list, call it "q"
    b) pop q off the open list
    c) generate q's 8 successors and set their parents to q
   
    d) for each successor
        i) if successor is the goal, stop search
          successor.g = q.g + distance between successor and q
          successor.h = distance from goal to successor (heuristics- Manhattan, Diagonal and Euclidean Heuristics)
          successor.f = successor.g + successor.h

        ii) if a node with the same position as successor is in the OPEN list which has a 
           lower f than successor, skip this successor

        iii) if a node with the same position as successor is in the CLOSED list which has
            a lower f than successor, skip this successor, otherwise, add  the node to the open list
     end (for loop)
  
    e) push q on the closed list
    end (while loop) 
)";

struct point
{
    point(int a = 0, int b = 0)
    {
        x = a;
        y = b;
    }
    bool operator==(const point& o) const
    {
        return o.x == x && o.y == y;
    }
    point operator+(const point& o) const
    {
        return point(o.x + x, o.y + y);
    }
    std::string to_string() const
    {
        std::stringstream strm;
        strm << "x=" << x << ",y=" << y;
        return strm.str();
    }
    int x, y;
};

struct map
{
    map()
    {
        char t[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 1, 1, 1, 0}, {0, 0, 1, 0, 0, 0, 1, 0},
                        {0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 1, 1, 1, 1, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
        w = h = 8;
        for (int r = 0; r < h; r++)
            for (int s = 0; s < w; s++)
                m[s][r] = t[r][s];
    }
    int operator()(int x, int y)
    {
        return m[x][y];
    }
    char m[8][8];
    int w, h;
};

struct node
{
    bool operator==(const node& o) const
    {
        return pos == o.pos;
    }
    bool operator==(const point& o) const
    {
        return pos == o;
    }
    bool operator<(const node& o) const
    {
        return h + cost < o.h + o.cost;
    }
    std::string to_string() const
    {
        std::stringstream strm;
        strm << "pos:" << pos.to_string() << "\tparent:" << parent.to_string()
             << "\tdist=" << h << ",cost=" << cost;
        return strm.str();
    }
    int full_cost() const
    {
    	return h + cost;
    }
    point pos, parent;
    int h; // heuristic - the estimated movement cost to move from that given square on the grid to the final destination.
    int cost;
};

class aStar
{
public:
    aStar()
    {
        neighbours[0] = point(-1, -1);
        neighbours[1] = point(1, -1);
        neighbours[2] = point(-1, 1);
        neighbours[3] = point(1, 1);
        neighbours[4] = point(0, -1);
        neighbours[5] = point(-1, 0);
        neighbours[6] = point(0, 1);
        neighbours[7] = point(1, 0);
    }

    int Manhattan_distance(const point& p)
    {
        return abs(end.x - p.x) + abs(end.y - p.y);
    }

    int Euclidean_distance(const point& p)
    {
        return std::hypot(end.x - p.x, end.y - p.y);
    }

    int heuristic(const point& p)
    {
        return Euclidean_distance(p);
        // need a better heuristic
        int x = end.x - p.x, y = end.y - p.y;
        return (x * x + y * y);
    }

    bool isValid(const point& p)
    {
        return (p.x > -1 && p.y > -1 && p.x < m.w && p.y < m.h);
    }

    bool is_pt_done(const point& p, int cost)
    {
        auto iter = std::find(closed.begin(), closed.end(), p);
        if (iter != closed.end())
        {
            if (iter->full_cost() < cost)
                return true;
            else
            {
                closed.erase(iter);
                return false;
            }
        }
        iter = std::find(open.begin(), open.end(), p);
        if (iter != open.end())
        {
            if (iter->full_cost() < cost)
                return true;
            else
            {
                open.erase(iter);
                return false;
            }
        }
        return false;
    }

    bool fillOpen(const node& n)
    {
        for (int x = 0; x < std::extent<decltype(neighbours)>::value; x++)
        {
            // one can make diagonals have different cost
            int stepCost = x < 4 ? 1 : 1;
            point neighbour = n.pos + neighbours[x];
            if (neighbour == end)
                return true;

            if (isValid(neighbour) && m(neighbour.x, neighbour.y) != 1)
            {
                int nc = stepCost + n.cost;
                int dist = heuristic(neighbour);
                if (!is_pt_done(neighbour, nc + dist))
                {
                    node m;
                    m.cost = nc;
                    m.h = dist;
                    m.pos = neighbour;
                    m.parent = n.pos;
                    open.push_back(m);
                }
            }
        }
        return false;
    }

    bool search(const point& s, const point& e, const map& mp)
    {
        node n;
        end = e;
        start = s;
        m = mp;
        n.cost = 0;
        n.pos = s;
        n.parent = 0;
        n.h = heuristic(s);
        open.push_back(n);
        while (!open.empty())
        {
            // open.sort();
            node n = open.front();
            open.pop_front();
            closed.push_back(n);
            if (fillOpen(n))
                return true;
        }
        return false;
    }

    int path(std::list<point>& path)
    {
        path.push_front(end);
        int cost = 1 + closed.back().cost;
        path.push_front(closed.back().pos);
        point parent = closed.back().parent;

        for (auto iter = closed.rbegin(); iter != closed.rend(); iter++)
        {
            if (iter->pos == parent && !(iter->pos == start))
            {
                path.push_front(iter->pos);
                parent = iter->parent;
            }
        }
        path.push_front(start);
        return cost;
    }

    map m;
    point end, start;
    point neighbours[8];
    std::list<node> open;
    std::list<node> closed;
};

int main(int argc, char* argv[])
{
    map m;
    point s, e(7, 7);
    aStar as;

    if (as.search(s, e, m))
    {
        std::list<point> path;
        int c = as.path(path);
        for (int y = -1; y < 9; y++)
        {
            for (int x = -1; x < 9; x++)
            {
                if (x < 0 || y < 0 || x > 7 || y > 7 || m(x, y) == 1)
                    std::cout << char(0xdb);
                else
                {
                    if (std::find(path.begin(), path.end(), point(x, y)) != path.end())
                        std::cout << "x";
                    else
                        std::cout << ".";
                }
            }
            std::cout << "\n";
        }

        std::cout << "\nPath cost " << c << ": ";
        for (auto i = path.begin(); i != path.end(); i++)
        {
            std::cout << "(" << (*i).x << ", " << (*i).y << ") ";
        }
    } else
    	std::cout << "No path\n";

    std::cout << "\n\n";
    return 0;
}
