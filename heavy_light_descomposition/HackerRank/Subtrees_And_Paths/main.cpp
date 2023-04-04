#include <iostream>
#include <vector>
using namespace std;

#define INF 2000000000

typedef long long ll;
typedef vector<ll> vi;
typedef vector<vi> vvi;

void lazy_update(vi &segment_tree, vi &lazy_tree, int L, int R, int current) {
    if(lazy_tree[current]) {
        segment_tree[current] += lazy_tree[current];
        if(L != R) {
            lazy_tree[2*current] += lazy_tree[current];
            lazy_tree[2*current + 1] += lazy_tree[current];
        }
        lazy_tree[current] = 0;
    }
}

void update_st(vi &segment_tree, vi &lazy_tree, int L, int R, int current, int updated_l, int updated_r, int value) {
    lazy_update(segment_tree, lazy_tree, L, R, current);
    if(updated_r < L || R < updated_l) {
        return;
    }
    if(updated_l<=L && R<=updated_r) {
        segment_tree[current] += value;
        if(L != R) {
            lazy_tree[2*current] += value;
            lazy_tree[2*current+1] += value;
        }
        return;
    }
    int m = (L+R)/2;
    update_st(segment_tree, lazy_tree, L, m, 2*current, updated_l, updated_r, value);
    update_st(segment_tree, lazy_tree, m + 1, R, 2*current + 1, updated_l, updated_r, value);
    segment_tree[current] = max(segment_tree[2*current], segment_tree[2*current+1]);
}

void query_st(vi &segment_tree, vi &lazy_tree, int L, int R, int current, int l, int r, ll &ans) {
    lazy_update(segment_tree, lazy_tree, L, R, current);
    if(R<l || r<L) {
        return;
    }
    if(l<=L && R<=r) {
        ans = max(ans, segment_tree[current]);
        return;
    }
    int m = (L+R)/2;
    query_st(segment_tree, lazy_tree, L, m, 2*current, l, r, ans);
    query_st(segment_tree, lazy_tree, m + 1, R, 2*current + 1, l, r, ans);
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

void hld(vvi &G, int current, int parent, vi &subtree_size, vi &id, int &count, vi &color, int paint, vi &segment_tree, vi &value) {
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
        hld(G, heavy_child, current, subtree_size, id, count, color, paint, segment_tree, value);
        for(int i=0; i<G[current].size(); i++) {
            int child = G[current][i];
            if(child != parent && child != heavy_child) {
                hld(G, child, current, subtree_size, id, count, color, child, segment_tree, value);
            }
        }
    }
}

ll query(int u, int v, vi &parent, vi &color, vi &depth, vi &segment_tree, vi &lazy_tree, vi &id) {
    ll ans = -INF;
    while(color[u] != color[v]) {
        if(depth[color[u]] > depth[color[v]]) {
            swap(u, v);
        }
        // so depth[u] <= depth[v]
        query_st(segment_tree, lazy_tree, 1, parent.size()-1, 1, id[color[v]], id[v], ans);
        v = parent[color[v]];
    }
    if(depth[u] > depth[v]) {
        swap(u, v);
    }
    query_st(segment_tree, lazy_tree, 1, parent.size()-1, 1, id[u], id[v], ans);
    return ans;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int n;
    cin >> n;

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
    vi color(n+1);
    vi segment_tree(4*n);
    vi lazy_tree(4*n);
    hld(G, 1, 0, subtree_size, id, count, color, 1, segment_tree, value);

    int q;
    cin >> q;
    for(int i=0; i<q; i++) {
        string query_type;
        int a, b;
        cin >> query_type >> a >> b;
        if(query_type == "add") {
            update_st(segment_tree, lazy_tree, 1, n, 1, id[a], id[a] + subtree_size[a] - 1, b);            
        } else {
            cout << query(a, b, parents, color, depth, segment_tree, lazy_tree, id) << '\n';
        }
    }
    return 0;
}
