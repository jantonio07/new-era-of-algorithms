#include <iostream>
#include <vector>
using namespace std;

#define INF 1000000000

typedef vector<int> vi;
typedef vector<vi> vvi;

void update_st(vi &segment_tree, int L, int R, int current, int updated_node, int new_value) {
    if(updated_node < L || R < updated_node) {
        return;
    }
    if(L == R) {
        segment_tree[current] = new_value ? L : INF;
        return;
    }
    int m = (L+R)/2;
    update_st(segment_tree, L, m, 2*current, updated_node, new_value);
    update_st(segment_tree, m + 1, R, 2*current + 1, updated_node, new_value);
    segment_tree[current] = min(segment_tree[2*current], segment_tree[2*current + 1]);  
}

void query_st(vi &segment_tree, int L, int R, int current, int l, int r, int &ans) {
    if(R<l || r<L) {
        return;
    }
    if(l<=L && R<=r) {
        ans = min(ans, segment_tree[current]);
        return;
    }
    int m = (L+R)/2;
    query_st(segment_tree, L, m, 2*current, l, r, ans);
    query_st(segment_tree, m + 1, R, 2*current + 1, l, r, ans);
}

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

void hld(vvi &G, int current, int parent, vi &subtree_size, vi &id, vi &id_inv, int &count, vi &color, int paint, vi &segment_tree, vi &value) {
    id[current] = ++count;
    id_inv[count] = current; 
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
        hld(G, heavy_child, current, subtree_size, id, id_inv, count, color, paint, segment_tree, value);
        for(int i=0; i<G[current].size(); i++) {
            int child = G[current][i];
            if(child != parent && child != heavy_child) {
                hld(G, child, current, subtree_size, id, id_inv, count, color, child, segment_tree, value);
            }
        }
    }
}

int query(int u, int v, vi &parent, vi &color, vi &depth, vi &segment_tree, vi &id, vi &id_inv) {
    int ans = INF;
    while(color[u] != color[v]) {
        if(depth[color[u]] > depth[color[v]]) {
            swap(u, v);
        }
        // so depth[u] <= depth[v]
        query_st(segment_tree, 1, parent.size()-1, 1, id[color[v]], id[v], ans);
        v = parent[color[v]];
    }
    if(depth[u] > depth[v]) {
        swap(u, v);
    }
    query_st(segment_tree, 1, parent.size()-1, 1, id[u], id[v], ans);
    ans = ans == INF ? -1 : id_inv[ans];
    return ans;
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
    vi value(n+1);
    for(int i=2; i<=n; i++) {
        int u, v;
        cin >> u >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }
    dfs(G, 1, 0, subtree_size, parents, depth);

    int count = 0;
    vi id(n+1);
    vi id_inv(n+1);
    vi color(n+1);
    vi segment_tree(4*n, INF);
    hld(G, 1, 0, subtree_size, id, id_inv, count, color, 1, segment_tree, value);

    for(int i=0; i<q; i++) {
        int a, b;
        cin >> a >> b;
        if(a == 0) {
            value[id[b]] = (value[id[b]] + 1)%2;
            update_st(segment_tree, 1, n, 1, id[b], value[id[b]]);
        } else {
            cout << query(1, b, parents, color, depth, segment_tree, id, id_inv) << '\n';
        }
    }
    return 0;
}
