#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
using namespace std;

const int N = 5000;

class DSU {
public:
    int n;  // array size
    vector<int> t;
    vector<vector<int>> r;

    int get(int x) {
        return t[x] == x ? x : t[x] = get(t[x]);
    }

    int unite(int x, int y) {
        x = get(x);
        y = get(y);
        if (x == y) return 0;
        if (r[x].size() <= r[y].size()) {
            t[x] = y;
            r[y].insert(r[y].end(), r[x].begin(), r[x].end());
        } else {
            t[y] = x;
            r[x].insert(r[x].end(), r[y].begin(), r[y].end());
        }
        return 1;
    }

    DSU(int n) : n(n), t() {
        t.resize(n);
        r.assign(n, {});
        for (int i = 0; i < n; i++) t[i] = i, r[i] = {i};
    }
};

DSU dsu(0);
vector<int> marked;
vector<int> used;
vector<int> paired_list;
vector<int> paired;
int timer = 0;
Input in;

void merge_dependent() {
    vector<bitset<N>> to_connect(in.n);
    for (int blocked = 1; blocked < in.n - 1; blocked++) {
        ++timer;
        queue<int> q;
        q.push(in.n - 1);
        used[in.n - 1] = timer;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : in.gr[v]) {
                if (to == blocked || used[to] == timer) continue;
                used[to] = timer;
                q.push(to);
            }
        }
        for (int i = 1; i < in.n - 1; i++) {
            if (i == blocked) continue;
            if (used[i] != timer) {
                to_connect[i][blocked] = 1;
            }
        }
    }
    for (int blocked = 1; blocked < in.n - 1; blocked++) {
        ++timer;
        queue<int> q;
        q.push(0);
        used[0] = timer;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : in.g[v]) {
                if (to == blocked || used[to] == timer) continue;
                used[to] = timer;
                q.push(to);
            }
        }
        for (int i = 1; i < in.n - 1; i++) {
            if (used[i] != timer) {
                if (to_connect[blocked][i]) {
                    dsu.unite(i, blocked);
                    marked[i] |= 2;
                    marked[blocked] |= 2;
                    paired_list.push_back(i);
                    paired_list.push_back(blocked);
                }
            }
        }
        if (used[in.n - 1] != timer) {
            dsu.unite(blocked, 0);
        }
        sort(paired_list.begin(), paired_list.end());
        paired_list.erase(unique(paired_list.begin(), paired_list.end()), paired_list.end());
    }
    for (auto x : paired_list) paired[x] = 1;
}

void connect_not_marked_to_paired() {
    vector<int> node_ids(in.n);
    iota(node_ids.begin(), node_ids.end(), 0);
    auto node_weights = in.w;
    sort(node_ids.begin(), node_ids.end(), [&node_weights](int a, int b) {
        return node_weights[a] > node_weights[b];
    });
    for (auto v : node_ids) {
        // if (paired[v]) continue;
        vector<int> region_v = dsu.r[dsu.get(v)];
        bool has_marked = false;
        for (auto u : region_v) has_marked |= marked[u] == 1;
        if (has_marked) continue;
        if (paired[v]) continue;
        for (int to : in.g[v]) {
            if (!paired[to] || dsu.get(to) == dsu.get(v)) continue;
            vector<int> region = dsu.r[dsu.get(to)];
            region.insert(region.end(), region_v.begin(), region_v.end());
            auto beta_pair = utils::get_region_beta_pair(in, region);
            if (1ll * beta_pair.first * 10 < beta_pair.second * 9) continue;
            // paired[v] = 1;
            dsu.unite(v, to);
            for (auto u : region_v) marked[u] &= 1;
            break;
        }
    }
}

void solve() {
    cin >> in;

    paired_list = {0, in.n - 1};
    paired.assign(in.n, 0);
    used.assign(in.n, 0);

    dsu = DSU(in.n);
    dsu.unite(0, in.n - 1);
    marked.assign(in.n, 0);
    for (int x : in.u) marked[x] = 1;
    marked[0] |= 2;

    merge_dependent();
    // connect_not_marked_to_paired();

    Output out;
    //
    out.regions.resize(in.n);
    for (int i = 0; i < in.n; i++) {
        out.regions[dsu.get(i)].push_back(i);
        // cerr << i << ' ' << dsu.get(i) << endl;
    }
    sort(out.regions.begin(), out.regions.end(), [](const vector<int> &a, const vector<int> &b) {
        return a.size() > b.size();
    });
    while (out.regions.back().size() == 0) out.regions.pop_back();
    for (const auto &region : out.regions) {
        if (region.size() == 1) {
            marked[region[0]] |= 2;
        }
    }
    //
    for (int i = 0; i < in.n; i++) {
        if (marked[i] == 2) {
            out.v.push_back(i);
        }
    }

    cout << out;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int t;
    cin >> t;
    while (t--) {
        in.init_test();
        solve();
    }
    return 0;
}