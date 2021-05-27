#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;
using Region = vector<int>;

namespace utils {};  // namespace utils

struct Edge {
    int u;
    int v;
};

struct Input {
    int n;
    vector<int> w;
    int k;
    vector<int> u;
    int m;
    vector<Edge> edges;
    vector<vector<int>> g;
    vector<vector<int>> gr;

    void init_test() {
        g.clear();
        edges.clear();
        gr.clear();
        u.clear();
        w.clear();
    }

    friend ostream& operator<<(ostream& stream, const Input& in) {
        stream << in.n;
        for (int x : in.w) stream << ' ' << x;
        stream << endl;
        stream << in.k;
        for (int x : in.u) stream << ' ' << x + 1;
        stream << endl;
        stream << in.m << endl;
        for (int i = 0; i < in.m; i++) {
            stream << in.edges[i].u + 1 << ' ' << in.edges[i].v + 1 << endl;
        }
        return stream;
    }

    friend istream& operator>>(istream& stream, Input& in) {
        stream >> in.n;
        in.w.resize(in.n);
        for (int& x : in.w) stream >> x;
        stream >> in.k;
        in.u.resize(in.k);
        for (int& x : in.u) {
            stream >> x;
            --x;
        }
        stream >> in.m;
        in.g.resize(in.n);
        in.gr.resize(in.n);
        in.edges.resize(in.m);
        for (int i = 0; i < in.m; i++) {
            stream >> in.edges[i].u >> in.edges[i].v;
            --in.edges[i].u;
            --in.edges[i].v;
            in.g[in.edges[i].u].push_back(in.edges[i].v);
            in.gr[in.edges[i].v].push_back(in.edges[i].u);
        }
        return stream;
    }

    void build_topsort(int v, const vector<vector<int>>& g, vector<int>& used, vector<int>& ans) const {
        used[v] = 1;
        for (int to : g[v]) {
            if (!used[to]) {
                build_topsort(to, g, used, ans);
            }
        }
        ans.push_back(v);
    }

    vector<int> get_topsort(bool reversed) const {
        vector<int> ans;
        vector<int> used(n, 0);
        build_topsort(reversed ? n - 1 : 0, reversed ? gr : g, used, ans);
        return ans;
    }
};

struct Output {
    vector<int> v;
    vector<Region> regions;

    Output() = default;

    void init_test() {
    }

    friend ostream& operator<<(ostream& stream, const Output& out) {
        stream << out.v.size();
        for (int x : out.v) stream << ' ' << x + 1;
        stream << endl;
        stream << out.regions.size() << endl;
        for (const auto& region : out.regions) {
            stream << region.size();
            for (auto v : region) {
                stream << ' ' << v + 1;
            }
            stream << endl;
        }
        return stream;
    }
    friend istream& operator>>(istream& stream, Output& out) {
        int q;
        stream >> q;
        out.v.resize(q);
        for (int& x : out.v) {
            stream >> x;
            --x;
        }
        int k;
        stream >> k;
        out.regions.resize(k);
        for (auto& region : out.regions) {
            int cnt;
            stream >> cnt;
            region.resize(cnt);
            for (auto& v : region) {
                stream >> v;
                --v;
            }
        }
        return stream;
    }
};

namespace utils {
const int N = 5000;

struct Checker {
    Input in;
    vector<bitset<N>> reachability_table;

    Checker() {}
    Checker(const Input& in) : in(in) {
        used.resize(in.n);
        build_reachability_table();
    }
    bool is_correct_region(const vector<int>& marked, const vector<int>& region) {
        bitset<N> region_bitset;
        for (auto v : region) region_bitset[v] = 1;
        for (auto v : region) {
            if (marked[v]) {
                if ((reachability_table[v] & region_bitset).count()) {
                    // cerr << "[Error] There is path that contains not all marked verticies from some region";
                    return false;
                }
            }
        }
        return true;
    }
    bool is_correct(const Output& out) {
        bitset<N> marked;
        for (int v : in.u) marked[v] = 1;

        for (int v : out.v) {
            if (marked[v]) {
                cerr << "[Error] Already marked vertex " << v + 1 << endl;
                return false;
            }
            marked[v] = 1;
        }

        {
            set<int> all;
            bitset<N> region_bitset;
            for (const auto& region : out.regions) {
                bool has_marked = false;
                for (auto v : region) region_bitset[v] = 1;
                for (auto v : region) {
                    if (all.count(v)) {
                        cerr << "[Error] Vertex " << v + 1 << " is more than in one region" << endl;
                        return false;
                    }
                    if (marked[v]) {
                        has_marked = true;
                    }
                    all.insert(v);
                }
                for (auto v : region) {
                    if (marked[v]) {
                        if ((reachability_table[v] & region_bitset).count()) {
                            cerr << "[Error] There is path that contains not all marked verticies from some region";
                            return false;
                        }
                    }
                }
                if (!has_marked) {
                    cerr << "[Error] Some region does not contain marked vertex" << endl;
                    return false;
                }
                for (auto v : region) region_bitset[v] = 0;
            }
            for (int i = 0; i < in.n; i++) {
                if (all.count(i) == 0) {
                    cerr << "[Error] Vertex " << i + 1 << " does not belong to any region" << endl;
                    return false;
                }
            }
        }
        return true;
    }
    int score(const Output& out) {
        if (!is_correct(out)) {
            return 0;
        }

        double min_beta = 1.0;
        for (const auto& region : out.regions) {
            pair<int, int> cur = get_region_beta_pair(region);
            // cerr << cur.first << ' ' << cur.second << endl;
            if (1ll * cur.first * 10 < 9 * cur.second) {
                cerr << "[Error] There is region with beta < 0.9" << endl;
                return 0;
            }
            min_beta = min(min_beta, 1.0 * cur.first / cur.second);
        }
        cerr << "[Info] Min beta = " << fixed << setprecision(8) << min_beta << endl;

        return out.regions.size();
    }

    pair<int, int> get_region_beta_pair(const vector<int>& region) {
        vector<int> cur_w(in.n, 0);
        for (int v : region) cur_w[v] = in.w[v];
        auto topsort = in.get_topsort(false);
        vector<pair<int, int>> dp(in.n);
        for (int v : topsort) {
            dp[v] = {1e9, 0};
            for (int to : in.g[v]) {
                if (dp[to].first != 0) dp[v].first = min(dp[v].first, dp[to].first);
                dp[v].second = max(dp[v].second, dp[to].second);
            }
            if (dp[v].first > dp[v].second) dp[v] = {0, 0};
            dp[v].first += cur_w[v];
            dp[v].second += cur_w[v];
        }
        return dp[0];
    }

private:
    vector<int> used;
    int timer = 0;

    void build_reachability_table() {
        reachability_table.clear();
        reachability_table.resize(in.n);

        vector<bitset<N>> reachability(in.n);
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
                if (used[i] == timer) {
                    reachability[blocked][i] = 1;
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
                if (used[i] == timer) {
                    if (reachability[blocked][i]) {
                        reachability_table[blocked][i] = 1;
                    }
                }
            }
        }
        // for (int i = 0; i < in.n; i++) {
        //     for (int j = 0; j < in.n; j++) {
        //         cout << reachability[i][j];
        //     }
        //     cout << endl;
        // }
    }
};

vector<pair<int, int>> get_minmax_path_length(const Input& in, bool reversed = false) {
    vector<pair<int, int>> ans(in.n);
    vector<int> used(in.n);

    function<void(int)> dfs;
    dfs = [&in, &ans, &used, reversed, &dfs](int v) {
        used[v] = true;
        const auto& adj = reversed ? in.gr[v] : in.g[v];

        ans[v] = {1e9, 0};
        for (auto to : adj) {
            if (!used[to]) {
                dfs(to);
            }
            if (ans[to].first < ans[v].first) {
                ans[v].first = ans[to].first;
            }
            if (ans[to].second > ans[v].second) {
                ans[v].second = ans[to].second;
            }
        }
        if (ans[v].first > ans[v].second) ans[v] = {0, 0};
        ans[v].first += in.w[v];
        ans[v].second += in.w[v];
    };

    for (int i = 0; i < in.n; i++) {
        if (!used[i]) {
            dfs(i);
        }
    }
    return ans;
}

Input input_generator() {
    Input in;
    in.n = 500;
    for (int i = 0; i < in.n; i++) in.w.push_back(rand() % 50);
    in.k = 0;
    for (int i = rand() % in.n; i < in.n; i += rand() % 50 + 1) {
        in.k++;
        in.u.push_back(i);
    }
    for (int i = in.n - 2; i >= 0; i--) {
        vector<int> all(in.n - i - 1);
        iota(all.begin(), all.end(), i + 1);
        random_shuffle(all.begin(), all.end());
        for (int j = 0; j < min<int>(all.size(), 50); j++) {
            in.edges.push_back({i, all[j]});
        }
    }
    in.m = in.edges.size();
    return in;
}
}  // namespace utils#include <bits/stdc++.h>

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
utils::Checker checker;

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
                to_connect[blocked][i] = 1;
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
                if (to_connect[i][blocked]) {
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
    vector<int> cur_w = in.w;
    iota(node_ids.begin(), node_ids.end(), 0);
    sort(node_ids.begin(), node_ids.end(), [&cur_w](int a, int b) {
        return cur_w[a] > cur_w[b];
    });
    for (auto v : node_ids) {
        // if (paired[v]) continue;
        vector<int> region_v = dsu.r[dsu.get(v)];
        bool has_marked = false;
        for (auto u : region_v) has_marked |= marked[u] > 0;
        // if (!has_marked) continue;
        // if (paired[v]) continue;
        for (int to : in.g[v]) {
            if ((dsu.get(to) == dsu.get(v))) continue;
            vector<int> region = dsu.r[dsu.get(to)];

            bool has_marked = false;
            for (auto u : region) has_marked |= marked[u] > 1;
            if (!has_marked) continue;

            region.insert(region.end(), region_v.begin(), region_v.end());
            auto beta_pair = checker.get_region_beta_pair(region);
            if (1ll * beta_pair.first * 10 < beta_pair.second * 9) continue;
            if (!checker.is_correct_region(marked, region)) continue;
            // paired[v] = 1;
            dsu.unite(v, to);
            for (auto u : region_v) marked[u] &= 1;
            break;
        }
    }
}

void solve() {
    cin >> in;
    checker = utils::Checker(in);

    paired_list = {0, in.n - 1};
    paired.assign(in.n, 0);
    used.assign(in.n, 0);

    dsu = DSU(in.n);
    dsu.unite(0, in.n - 1);
    marked.assign(in.n, 0);
    for (int x : in.u) marked[x] = 1;
    marked[0] |= 2;

    merge_dependent();
    connect_not_marked_to_paired();

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
    // {
    //     cout << 3 << endl;

    //     for (int i = 0; i < 3; i++) {
    //         Input in = utils::input_generator();
    //         cout << in << endl;
    //     }
    //     return 0;
    // }
    int t;
    cin >> t;
    while (t--) {
        in.init_test();
        solve();
    }
    return 0;
}
