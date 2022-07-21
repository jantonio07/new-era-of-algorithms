#include <iostream>
#include <vector>
using namespace std;


void dfs(vector<vector<int> > &G, vector<int> &visited, int node) {
	if(visited[node]) {
		return;
	}

	visited[node]++;

	for(int i=0; i<G[node].size(); i++) {
		dfs(G, visited, G[node][i]);
	}
}

void solve() {
	char n;
	cin >> n;
	n -= 'A' - 1;

	// create graph
	vector<vector<int> > G(n);
	string edge;

	getline(cin, edge);
	while(getline(cin, edge) && edge.length() > 0) {
		int u = edge[0] - 'A';
		int v = edge[1] - 'A';

		G[u].push_back(v); 
		G[v].push_back(u); 
	}

	// find connected components
	vector<int> visited(n);
	int ans = 0;

	for(int i=0; i<n; i++) {
		if(!visited[i]) {
			ans++;
			dfs(G, visited, i);
		}
	}

	// print ans
	cout << ans << '\n';
}

int main() {
	cin.tie(0);
	ios::sync_with_stdio(false);

	int t;
	cin >> t;

	while(t--) {
		solve();
		if(t) {
			cout << '\n';
		}
	}

	return 0;
}
