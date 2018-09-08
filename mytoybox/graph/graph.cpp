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
)";
