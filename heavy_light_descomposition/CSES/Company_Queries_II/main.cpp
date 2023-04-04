#include <iostream>
#include <vector>
using namespace std;

typedef vector<int> vi;
typedef vector<vi> vvi;

void dfs(vvi &G, int current, int parent, vi &subtree_size, vi &parents, vi &depth) {
    subtree_size[current] = 1;
    parents[current] = parent;
    for(int i=0; i<G[current].size(); i++) {
        int child = G[current][i];
        if(child != parent) {
            depth[child] = depth[current] + 1;
            dfs(G, child, current, subtree_size, parents, depth);
            subtree_size[current] += subtree_size[child];
        }
    }
}

void hld(vvi &G, int current, int parent, vi &subtree_size, vi &id, int &count, vi &color, int paint) {
    id[current] = ++count;
    color[current] = paint;
    int heavy_child = 0, max_size = 0;
    for(int i=0; i<G[current].size(); i++) {
        int child = G[current][i];
        if(child != parent) {
            if(max_size < subtree_size[child]) {
                heavy_child = child;
                max_size = subtree_size[child];
            }
        }
    }
    if(heavy_child) {
        hld(G, heavy_child, current, subtree_size, id, count, color, paint);
        for(int i=0; i<G[current].size(); i++) {
            int child = G[current][i];
            if(child != parent && child != heavy_child) {
                hld(G, child, current, subtree_size, id, count, color, child);
            }
        }
    }
}

int query(int u, int v, vi &parent, vi &color, vi &depth) {
    while(color[u] != color[v]) {
        if(depth[color[u]] > depth[color[v]]) {
            swap(u, v);
        }
        // so depth[u] <= depth[v]
        v = parent[color[v]];
    }
    return depth[u] < depth[v] ? u : v;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n, q;
    cin >> n >> q;

    vvi G(n+1);
    vi subtree_size(n+1);
    vi parents(n+1);
    vi depth(n+1);
    for(int i=2; i<=n; i++) {
        int u;
        cin >> u;
        G[i].push_back(u);
        G[u].push_back(i);
    }
    dfs(G, 1, 0, subtree_size, parents, depth);

    int count = 0;
    vi id(n+1);
    vi color(n+1);
    hld(G, 1, 0, subtree_size, id, count, color, 1);

    for(int i=0; i<q; i++) {
        int u, v;
        cin >> u >> v;
        cout << query(u, v, parents, color, depth) << '\n';
    }

    return 0;
}
