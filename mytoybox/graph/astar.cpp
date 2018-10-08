#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
// refer to https://rosettacode.org/wiki/A*_search_algorithm#C.2B.2B
struct point
{
    point(int a = 0, int b = 0)
    {
        x = a;
        y = b;
    }
    bool operator==(const point& o)
    {
        return o.x == x && o.y == y;
    }
    point operator+(const point& o) const
    {
        return point(o.x + x, o.y + y);
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
    bool operator==(const node& o)
    {
        return pos == o.pos;
    }
    bool operator==(const point& o)
    {
        return pos == o;
    }
    bool operator<(const node& o)
    {
        return dist + cost < o.dist + o.cost;
    }
    point pos, parent;
    int dist, cost;
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

    bool existPoint(const point& p, int cost)
    {
        auto i = std::find(closed.begin(), closed.end(), p);
        if (i != closed.end())
        {
            if ((*i).cost + (*i).dist < cost)
                return true;
            else
            {
                closed.erase(i);
                return false;
            }
        }
        i = std::find(open.begin(), open.end(), p);
        if (i != open.end())
        {
            if ((*i).cost + (*i).dist < cost)
                return true;
            else
            {
                open.erase(i);
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
                if (!existPoint(neighbour, nc + dist))
                {
                    node m;
                    m.cost = nc;
                    m.dist = dist;
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
        n.dist = heuristic(s);
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

        for (std::list<node>::reverse_iterator i = closed.rbegin(); i != closed.rend();
             i++)
        {
            if ((*i).pos == parent && !((*i).pos == start))
            {
                path.push_front((*i).pos);
                parent = (*i).parent;
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
        for (std::list<point>::iterator i = path.begin(); i != path.end(); i++)
        {
            std::cout << "(" << (*i).x << ", " << (*i).y << ") ";
        }
    } else
    	std::cout << "No path\n";

    std::cout << "\n\n";
    return 0;
}
