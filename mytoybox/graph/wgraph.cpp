#include <vector>
#include <iostream>
#include <priority_queue>

using namespace std;

struct edgenode {
	int y;
	int weight;
	edgenode* next;
	bool operator<(const edgenode& rhs) const
	{
		if (weight != rhs.weight)
			return weight > rhs.weight;
		return y > rhs.y;
	}
};

struct graph {
	vector<edgenode*> edges;
	vector<int> out_degrees;
	int nedges;
	bool directed;
};

struct graph2 {
	vector<priority_queue<edgenode>> edges;
	vector<int> out_degrees;
	int nedges;
	bool directed;
};

const char* bellman_ford = R"(
relax(u, v, w)
	if v.d > u.d + w(u,v)
		v.d = u.d + w(u,v)
		v.parent = u

init_single_source(G, s)
	for v in G.V
		v.d = inf
		v.parent = nil
	s.d = 0

bellman-ford(G, w, s)
	init_single_source(G, s)
	for i=1 to |G.V|-1
		for edge (u,v) in G.E
			relax(u, v, w)
	for edge (u,v) in G.E
		if v.d > u.d + w(u,v)
			return false; // negative cycle
	return true; // no negative cycle

// IOI	O(V*E)
for (int i = 1; i <= n; i++) 
	distance[i] = INF;
distance[source] = 0;
for (int i = 1; i <= n-1; i++) {
	for (auto e : edges) {
		int a, b, w;
		tie(a, b, w) = e;
		distance[b] = min(distance[b], distance[a]+w);
	}
}

)";

const char* topological_sort=R"(
topological_sort(G) // complexity: O(V+E)
	call dfs(G) to compute finishing time v.f for each vertex v
	as each vertex is finished, insert it onto the front of a linked list
	return the linked list of vertices
)";

const char* disjoint_set=R"(
make_set(x)
	x.parent = x		
	x.rank = 0

union_set(x, y)	// union by rank, O(m*log n)
	if x.rank > y.rank
		y.parent = x
	else x.parent = y
		if x.rank == y.rank
			y.rank++

find_set(x)
	if x.parent != x
		x.parent = find_set(x.parent) // path compression
	return x.parent
)";

const char* dfs=R"(
void dfs(graph* g, int v)
{
	if (finished) 
		return;
	discovered[v] = true;
	entry_time[v] = ++time;
	process_vertex_early(v);
	edgenode* p = g->edges[v];
	while (p) {
		y = p->y; // end vertex
		if (discovered[y] == false) {
			parent[y] = v;
			process_edge(v, y);
			dfs(g, y);
		} else if ((!processed[y]) || g->directed)
			process_edge(v, y);
		if (finished)
			return;
		p = p->next; // next edge
	}
	process_vertex_late(v);
	exit_time[v] = ++time;
	processed[v] = true;
}
)";

const char* topological_sort=R"(
find_cycle(int x, int y) // undirected graph
{
	if (parent[x] != y) { // any back edge going from x to an ancestor y creates a cycle with the tree path from y to x
		// cycle from y to x
		find_path(y, x, parent);
	}
}

int edge_classification(x, y)
{
	if (parent[y] ==x) return TREE;
	if (discovered[y] && !processed[y]) return BACK;
	if (processed[y] && (entry_time[y]>entry_time[x])) return FORWARD;
	if (processed[y] && (entry_time[y]<entry_time[x])) return CROSS;
}

proc_edge(int x, int y)
{
	int class = edge_classification(x, y);
	return class;
}

dfs_graph(G)
	for each vertex u in V|G| do
		state[u] = undiscovered
			for each vertex u in V|G| do
				if state[u] == undiscovered 
					initialize new component
					dfs(G,u)

topsort(graph* g)
{
	init_stack(&sorted);
	for (int i=1; i<g->nvertices; ++i)
		if (discovered[i] == false)
			dfs(g, i);
	print_stack(&sorted); // report topological order
}

)";

const char* MST_KRUSKAL=R"(
MST-KRUSKAL(G, w)
	A = set(); // edge set of the MST
	for v : G.V
		make_set(v)
	sort(G.E) into nondecreasing order by w
	for edge(u,v) : G.E 
		if find_set(u) != find_set(v)
			A.insert(edge(u,v))
			union(u,v)
	return A
)";

const char* MST_PRIM=R"(
MST-PRIM(G, w, root)
	for u : G.V
		u.key = INF
		u.parent = null
	root.key = 0
	Q = G.V	// Q is min-priority queue, better using Fibonacci heap
	while !Q.empty()
		u = Q.extract_min()
		for v : G.Adj[u]
			if v in Q and w(u,v) < v.key
				v.parent = u
				v.key = w(u,v)

when the algorithm terminates, the MST A for G is 
A = {(v, v.parent) : v in V - {root}}
)";
