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

pair<int, int> get_region_beta_pair(const Input& in, const vector<int>& region) {
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

int score(const Input& in, const Output& out) {
    vector<bool> marked(in.n, false);
    for (int v : in.u) marked[v] = true;

    for (int v : out.v) {
        if (marked[v]) {
            cerr << "[Error] Already marked vertex " << v + 1 << endl;
            return 0;
        }
        marked[v] = true;
    }

    {
        set<int> all;
        for (const auto& region : out.regions) {
            bool has_marked = false;
            for (auto v : region) {
                if (all.count(v)) {
                    cerr << "[Error] Vertex " << v + 1 << " is more than in one region" << endl;
                    return 0;
                }
                if (marked[v]) {
                    has_marked = true;
                }
                all.insert(v);
            }
            if (!has_marked) {
                cerr << "[Error] Some region does not contain marked region" << endl;
                return 0;
            }
        }
        for (int i = 0; i < in.n; i++) {
            if (all.count(i) == 0) {
                cerr << "[Error] Vertex " << i + 1 << " does not belong to any region" << endl;
                return 0;
            }
        }
    }

    double min_beta = 1.0;
    for (const auto& region : out.regions) {
        pair<int, int> cur = get_region_beta_pair(in, region);
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

}  // namespace utils