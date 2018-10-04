
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

using namespace std;

const char* bfs=R"(
bfs(G,s)
	for each v in G.V - {s}
		v.color = white
		v.parent = null
		v.d = INF
	s.color = gray
	s.parent = null
	s.d = 0
	Q.push(s)
	while !Q.empty()
		u = Q.top()
		Q.pop()
		u.color = gray
		for each v in G.Adj[u]
			if v.color == white
				Q.push(v)
				v.color = gray
				v.parent = u
				v.d = u.d + 1
		u.color = black

print_path(G, s, v)
	if s == v
		print s
	else if v.parent == null
		print "no path"
	else
		print_path(G, s, v.parent)
	print v

dfs(G)
	for each u in G.V
		u.color=white
		u.parent = null
	time = 0
	for each u in G.V
		if u.color==white
			dfs-visit(G, u)

dfs-visit(G, u)
	u.d = ++time	// discover time
	u.color = gray
	for each v in G.Adj[u]
		if v.color==white
			v.parent = u
			dfs-visit(G, v)
	u.color = black
	u.f = ++time	// finish time

topological-sort(G)
	dfs(G) to calculate finish time
	push_front when each vertex is finished process
	return linked list

strong-connected-components(G)
	DFS(G) calculate finish time
	compute transpose G
	DFS( transpose G), in the main dfs loop, consider vertex in order of decreasing u.f
	output vertices of each tree calculated in previous step as SCC.	

// single source to all vertices shorted path
BellmanFord(list vertices, list edges, vertex source)
   // This implementation takes in a graph, represented as
   // lists of vertices and edges, and fills two arrays
   // (distance and predecessor) about the shortest path
   // from the source to each vertex
   
   // Step 1: initialize graph
   // At the beginning , all vertices have a weight of infinity
   vector<int> distance(std::size(vertices), numeric_limits<int>::max());
   vector<int> predecessor(std::size(vertices), -1);

   distance[source] = 0              // The weight is zero at the source
   
   // Step 2: relax edges repeatedly
   for i from 1 to size(vertices)-1:
       for each edge (u, v) with weight w in edges:
           if distance[u] + w < distance[v]:
               distance[v] = distance[u] + w
               predecessor[v] = u
   
   // Step 3: check for negative-weight cycles
   for each edge (u, v) with weight w in edges:
       if distance[u] + w < distance[v]:
           error "Graph contains a negative-weight cycle"
   
   return distance, predecessor

Floyd-Warshall	(all pair shortest paths)
for (int i=1; i<=n; ++i)
	for (int j=1; j<=n; ++j)
	{
		if (i==j)	distance[i][j] = 0;
		else if (adj[i][j])	distance[i][j] = adj[i][j];
		else	distance[i][j] = std::numeric_limits::infinity();
	}

for (int k=1; k<=n; ++k)
	for (int i=1; i<=n; ++i)
		for (int j=1; j<=n; ++j)
			distance[i][j] = min(distance[i][j], distance[i][k]+distance[k][j];


)";

void floydWarshall (vector<vector<int>> graph)
{
	int V = graph.size();
    /* dist[][] will be the output matrix that will finally have the shortest
      distances between every pair of vertices */
    auto dist = graph;
    /* Initialize the solution matrix same as input graph matrix. Or
       we can say the initial values of shortest distances are based
       on shortest paths considering no intermediate vertex. */
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            dist[i][j] = graph[i][j];

    /* Add all vertices one by one to the set of intermediate vertices.
      ---> Before start of an iteration, we have shortest distances between all
      pairs of vertices such that the shortest distances consider only the
      vertices in set {0, 1, 2, .. k-1} as intermediate vertices.
      ----> After the end of an iteration, vertex no. k is added to the set of
      intermediate vertices and the set becomes {0, 1, 2, .. k} */
    for (int k = 0; k < V; k++)
    {
        // Pick all vertices as source one by one
        for (int i = 0; i < V; i++)
        {
            // Pick all vertices as destination for the
            // above picked source
            for (int j = 0; j < V; j++)
            {
                // If vertex k is on the shortest path from
                // i to j, then update the value of dist[i][j]
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

}

struct Coordinate
{
    bool operator==(const Coordinate& that) const
    {
        return x == that.x && y == that.y;
    }

    int x, y;
};

enum { kWhite, kBlack } Color;

// Perform DFS to find a feasible path.
bool SearchMazeHelper(const Coordinate& cur, const Coordinate& destination,
                      vector<vector<Color>>& maze, vector<Coordinate>& path)
{
    // Checks cur is within maze and is a white pixel.
    if (cur.x < 0 || cur.x >= size(maze) || cur.y < 0 || cur.y >= size(maze[cur.x]) ||
        maze[cur.x][cur.y] != kWhite)
    {
        return false;
    }
    path.emplace_back(cur);
    maze[cur.x][cur.y] = kBlack;
    if (cur == destination)
    {
        return true;
    }

    const Coordinate next_moves{
        Coordinate{cur.x, cur.y + 1}, Coordinate{cur.x, cur.y - 1},
        Coordinate{cur.x + 1, cur.y}, Coordinate{cur.x - 1, cur.y}};

    if (std::any_of(begin(next_moves), end(next_moves), [](const Coordinate& next_pos) {
            return SearchMazeHelper(next_pos, destination, maze, path);
        }))
        return true;

    // Cannot find a path, remove the entry added in path.emplace_back(cur).
    path.pop_back();
    return false;
}

vector<Coordinate> SearchMaze(vector<vector<Color>> maze, const Coordinate& s,
                              const Coordinate& e)
{
    vector<Coordinate> path;
    SearchMazeHelper(s, e, maze, path);
    return path;
}

// dead lock detection
struct Edge
{
    int from;
    int to;
};

struct Vertex
{
    enum Color
    {
        kWhite,
        kGray,
        kBlack
    } color = kWhite;

    vector<Vertex*> edges;
};

bool HasCycle(Vertex* cur)
{
    // Visiting a gray vertex means a cycle.
    if (cur->color == Vertex::kGray)
    {
        return true;
    }

    cur->color = Vertex::kGray; // Marks current vertex as a gray one.
    // Traverse the neighbor vertices.
    for (Vertex*& next : cur->edges)
    {
        if (next->color != Vertex::kBlack && HasCycle(next))
        {
            return true;
        }
    }
    cur->color = Vertex::kBlack; // Marks current vertex as black.
    return false;
}

bool IsDeadlocked(vector<Vertex>* graph)
{
    return any_of(begin(*graph), end(*graph), [](Vertex& vertex) {
        return vertex.color == Vertex::kWhite && HasCycle(&vertex);
    });
}
