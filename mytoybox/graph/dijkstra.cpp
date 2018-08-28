//#include <bits/stdc++.h>
// refer to https://www.quora.com/What-is-the-most-simple-efficient-C++-code-for-Dijkstras-shortest-path-algorithm
#include <vector>
#include <utility>
#include <limits>
#include <iostream>
#include <memory>
#include <queue>
using namespace std;

using vi = vector<int>;
using pii = pair<int,int>;
using vii = vector< pii >;

 // for storing the distance of every other node from source.
/*==========================================*/
void Dijkstra(vii* G, // adjacency list
		int source,
		int N, vi& Dist)
{
    priority_queue<pii, vector<pii>, greater<pii>> Q; // min heap, pii.first: dist, pii.second: vertex
    Dist.assign(N, std::numeric_limits<int>::max());
    Dist[source] = 0;
    Q.push({0, source});
    while (!Q.empty())
    {
        int u = Q.top().second;
        Q.pop();
        for (auto& c : G[u])
        {
            int v = c.first; // end vertex
            int w = c.second; // edge distance
            if (Dist[v] > Dist[u] + w)
            {
                Dist[v] = Dist[u] + w;
                Q.push({Dist[v], v});
            }
        }
    }
}

/*===========================================*/
int main()
{
    int N, M, u, v, w, source; // N-total no of nodes, M-no. of edges,
    cin >> N >>
        M; // u,v and w are the end vertices and the weight associated with an edge
    auto graph=std::make_unique<vii[]>( N + 1 );;

    for (int i = 0; i < M; ++i)
    {
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }
    cin >> source;
    vi Dist;
    Dijkstra(graph.get(), source, N, Dist);

    for (int i = 0; i < N; i++)
        cout << Dist[i] << " ";
    cout << endl;

    return 0;
}
