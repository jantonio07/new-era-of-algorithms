#include <iostream>
#include <vector>
using namespace std;

#define INF 2000000000

typedef long long ll;
typedef vector<ll> vi;
typedef vector<vi> vvi;

void lazy_update(vi &segment_tree, vi &lazy_tree, int L, int R, int current) {
    if(lazy_tree[current]) {
        segment_tree[current] = (R-L+1) - segment_tree[current];
        if(L != R) {
            lazy_tree[2*current] = (1 + lazy_tree[2*current])%2;
            lazy_tree[2*current+1] = (1 + lazy_tree[2*current+1])%2;
        }
        lazy_tree[current] = 0;
    }
}

void update_st(vi &segment_tree, vi &lazy_tree, int L, int R, int current, int updated_l, int updated_r) {
    lazy_update(segment_tree, lazy_tree, L, R, current);
    if(updated_r < L || R < updated_l) {
        return;
    }
    if(updated_l<=L && R<=updated_r) {
        segment_tree[current] = (R-L+1) - segment_tree[current];
        if(L != R) {
            lazy_tree[2*current] = (1 + lazy_tree[2*current])%2;
            lazy_tree[2*current+1] = (1 + lazy_tree[2*current+1])%2;
        }
        return;
    }
    int m = (L+R)/2;
    update_st(segment_tree, lazy_tree, L, m, 2*current, updated_l, updated_r);
    update_st(segment_tree, lazy_tree, m + 1, R, 2*current + 1, updated_l, updated_r);
    segment_tree[current] = segment_tree[2*current] + segment_tree[2*current+1];
}

ll query_st(vi &segment_tree, vi &lazy_tree, int L, int R, int current, int l, int r) {
    lazy_update(segment_tree, lazy_tree, L, R, current);
    if(R<l || r<L) {
        return 0;
    }
    if(l<=L && R<=r) {
        return segment_tree[current];
    }
    int m = (L+R)/2;
    int lr = query_st(segment_tree, lazy_tree, L, m, 2*current, l, r);
    int rr = query_st(segment_tree, lazy_tree, m + 1, R, 2*current + 1, l, r);
    return lr + rr;
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

void hld(vvi &G, int current, int parent, vi &subtree_size, vi &id, int &count, vi &color, int paint, vi &segment_tree, vi &value, vi &lazy_tree, vi &children_sum, vi &heavy_children, vi &is_heavy_node) {
    id[current] = ++count;
    color[current] = paint;
    if(value[current]) {
        update_st(segment_tree, lazy_tree, 1, G.size()-1, 1, id[current], id[current]);
    }
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
        heavy_children[current] = heavy_child;
        is_heavy_node[heavy_child] = 1;
        hld(G, heavy_child, current, subtree_size, id, count, color, paint, segment_tree, value, lazy_tree, children_sum, heavy_children, is_heavy_node);
        for(int i=0; i<G[current].size(); i++) {
            int child = G[current][i];
            if(child != parent && child != heavy_child) {
                hld(G, child, current, subtree_size, id, count, color, child, segment_tree, value, lazy_tree, children_sum, heavy_children, is_heavy_node);
                ll white_nodes = query_st(segment_tree, lazy_tree, 1, G.size() - 1, 1, id[child], id[child] + subtree_size[child] - 1);
                children_sum[current] += white_nodes*(white_nodes - 1)/2;
            }
        }
    }
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
    for(int i=1;i<=n; i++) {
        cin >> value[i];
    }
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
    vi children_sum(4*n);
    vi heavy_child(n+1);
    vi is_heavy_node(n+1);
    hld(G, 1, 0, subtree_size, id, count, color, 1, segment_tree, value, lazy_tree, children_sum, heavy_child, is_heavy_node);

    for(int i=0; i<q; i++) {
        int a, b;
        cin >> a >> b;
        if(a == 1) {
            int v = b;
            while(color[v] != color[1]) {
                ll white_nodes = query_st(segment_tree, lazy_tree, 1, n, 1, id[color[v]], id[color[v]] + subtree_size[color[v]] - 1);
                cout << (is_heavy_node[color[v]] ? "HEAVY" : "NO HEAVY") << '\n';
                children_sum[parents[color[v]]] -= white_nodes*(white_nodes-1)/2;
                v = parents[color[v]];
            }

            v = b;
            while(color[v] != color[1]) {
                update_st(segment_tree, lazy_tree, 1, parents.size() - 1, 1, id[color[v]], id[v]);            
                v = parents[color[v]];
            }
            update_st(segment_tree, lazy_tree, 1, parents.size() - 1, 1, id[1], id[v]);

            v = b;
            while(color[v] != color[1]) {
                ll white_nodes = query_st(segment_tree, lazy_tree, 1, n, 1, id[color[v]], id[color[v]] + subtree_size[color[v]] - 1);
                children_sum[parents[color[v]]] += white_nodes*(white_nodes-1)/2;
                v = parents[color[v]];
            }
        } else {
            ll white_nodes = query_st(segment_tree, lazy_tree, 1, n, 1, id[b], id[b] + subtree_size[b] - 1);
            ll ans = white_nodes*(white_nodes-1)/2 - children_sum[b];
            if(heavy_child[b]) {
                ll heavy_query = query_st(segment_tree, lazy_tree, 1, G.size() - 1, 1, id[heavy_child[b]], id[heavy_child[b]] + subtree_size[heavy_child[b]] - 1);
                ans -= heavy_query*(heavy_query - 1)/2;
            }
            cout << ans << '\n';
        }
    }
    return 0;
}
