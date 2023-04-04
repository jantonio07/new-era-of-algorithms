#include <iostream>
#include <vector>
using namespace std;

#define MOD 998244353

typedef long long ll;
typedef pair<ll, ll> ii;
typedef vector<ii> vii;
typedef vector<ll> vi;
typedef vector<vi> vvi;

ii compose_functions(ii x, ii y) {
    ii composition; 
    composition.first = (x.first*y.first)%MOD;
    composition.second = ((x.first*y.second)%MOD + x.second)%MOD;
    return composition;
}

void update_st(vii &segment_tree_lr, vii &segment_tree_rl, int L, int R, int current, int updated_node, ii coefs) {
    if(updated_node < L || R < updated_node) {
        return;
    }
    if(L == R) {
        segment_tree_lr[current] = coefs;
        segment_tree_rl[current] = coefs;
        return;
    }
    int m = (L+R)/2;
    update_st(segment_tree_lr, segment_tree_rl, L, m, 2*current, updated_node, coefs);
    update_st(segment_tree_lr, segment_tree_rl, m + 1, R, 2*current + 1, updated_node, coefs);
    // compose functions
    segment_tree_lr[current] = compose_functions(segment_tree_lr[2*current], segment_tree_lr[2*current+1]);
    segment_tree_rl[current] = compose_functions(segment_tree_rl[2*current+1], segment_tree_rl[2*current]);
}

ii query_st_1(vii &segment_tree_lr, int L, int R, int current, int l, int r) {
    if(R<l || r<L) {
        return ii(1, 0);
    }
    if(l<=L && R<=r) {
        return segment_tree_lr[current];
    }
    int m = (L+R)/2;
    ii lc = query_st_1(segment_tree_lr, L, m, 2*current, l, r);
    ii rc = query_st_1(segment_tree_lr, m + 1, R, 2*current + 1, l, r);
    return compose_functions(lc, rc);
}

ii query_st_2(vii &segment_tree_rl, int L, int R, int current, int l, int r) {
    if(R<l || r<L) {
        return ii(1, 0);
    }
    if(l<=L && R<=r) {
        return segment_tree_rl[current];
    }
    int m = (L+R)/2;
    ii lc = query_st_2(segment_tree_rl, L, m, 2*current, l, r);
    ii rc = query_st_2(segment_tree_rl, m + 1, R, 2*current + 1, l, r);
    return compose_functions(rc, lc);
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

void hld(vvi &G, int current, int parent, vi &subtree_size, vi &id, int &count, vi &color, int paint, vii &segment_tree_lr, vii &segment_tree_rl, vii &value) {
    id[current] = ++count;
    color[current] = paint;
    update_st(segment_tree_lr, segment_tree_rl, 1, G.size()-1, 1, id[current], value[current]);
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
        hld(G, heavy_child, current, subtree_size, id, count, color, paint, segment_tree_lr, segment_tree_rl, value);
        for(int i=0; i<G[current].size(); i++) {
            int child = G[current][i];
            if(child != parent && child != heavy_child) {
                hld(G, child, current, subtree_size, id, count, color, child, segment_tree_lr, segment_tree_rl, value);
            }
        }
    }
}

int LCA(int u, int v, vi &parent, vi &color, vi &depth) {
    while(color[u] != color[v]) {
        if(depth[color[u]] > depth[color[v]]) {
            swap(u, v);
        }
        // so depth[u] <= depth[v]
        v = parent[color[v]];
    }
    return depth[u] < depth[v] ? u : v;
}

ll query(int u, int v, int x, vi &parent, vi &color, vi &depth, vii &segment_tree_lr, vii &segment_tree_rl, vi &id) {
    int lca = LCA(u, v, parent, color, depth);
    ii f_lr, f_rl, f;
    f_lr.first = f_rl.first = 1;
    // from u to lca
    while(color[u] != color[lca]) {
        f = query_st_1(segment_tree_lr, 1, parent.size()-1, 1, id[color[u]], id[u]);
        f_lr = compose_functions(f, f_lr);
        u = parent[color[u]];
    }
    f = query_st_1(segment_tree_lr, 1, parent.size()-1, 1, id[lca]+1, id[u]);
    f_lr = compose_functions(f, f_lr);

    // from lca to v
    while(color[v] != color[lca]) {
        f = query_st_2(segment_tree_rl, 1, parent.size()-1, 1, id[color[v]], id[v]);
        f_rl = compose_functions(f_rl, f);
        v = parent[color[v]];
    }
    f = query_st_2(segment_tree_rl, 1, parent.size()-1, 1, id[lca], id[v]);
    f_rl = compose_functions(f_rl, f);

    ll ans = ((f_lr.first*x)%MOD + f_lr.second)%MOD;
    ans = ((f_rl.first*ans)%MOD + f_rl.second)%MOD;
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
    vii value(n+1);
    for(int i=1; i<=n; i++) {
        cin >> value[i].first >> value[i].second;
    }
    for(int i=2; i<=n; i++) {
        int u, v;
        cin >> u >> v;
        u++, v++;
        G[u].push_back(v);
        G[v].push_back(u);
    }
    dfs(G, 1, 0, subtree_size, parents, depth);

    int count = 0;
    vi id(n+1);
    vi color(n+1);
    vii segment_tree_lr(4*n);
    vii segment_tree_rl(4*n);
    hld(G, 1, 0, subtree_size, id, count, color, 1, segment_tree_lr, segment_tree_rl, value);
    
    for(int i=0; i<q; i++) {
        int query_type;
        cin >> query_type;
        if(query_type == 0) {
            int p;
            cin >> p;
            p++;
            cin >> value[p].first >> value[p].second;
            update_st(segment_tree_lr, segment_tree_rl, 1, n, 1, id[p], value[p]);
        } else {
            int u, v, x;
            cin >> u >> v >> x;
            u++, v++;
            cout << query(u, v, x, parents, color, depth, segment_tree_lr, segment_tree_rl, id) << '\n';
        }
    }
    return 0;
}
