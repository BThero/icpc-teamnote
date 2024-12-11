/*
	adj[v] stores indices of edges
	E[i] = {u, v}
*/
vector<int> stk;
int tmr;
 
void dfs(int v, int p = -1) {
  tin[v] = low[v] = ++tmr;
  stk.push_back(v);
 
  for (int id : adj[v]) {
    if (id == p) {
      continue;
    }
    int to = E[id][0] ^ E[id][1] ^ v;
    if (!tin[to]) {
      dfs(to, id);
      low[v] = min(low[v], low[to]);
      if (low[to] >= tin[v]) {
        vector<int> bc{v};
        while (bc.back() != to) {
          bc.push_back(stk.back());
          stk.pop_back();
        }
		// do something with biconnected component bc
      }
      if (low[to] > tin[v]) {
		// edge id is a bridge
      }
    } else {
      low[v] = min(low[v], tin[to]);
    }
  }
}