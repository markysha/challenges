#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;
using Region = vector<int>;

namespace utils {};  // namespace utils

struct Bitset {
    vector<uint64_t> data;

    Bitset(int n) {
        data.resize((n + 63) / 64);
    }

    Bitset(const Bitset& a) {
        data = a.data;
    }

    Bitset() {}

    int operator[](int pos) {
        return (data[pos / 64] >> (pos & 63)) & 1;
    }

    inline void set(int pos, int val) {
        int cur = (data[pos / 64] >> (pos & 63)) & 1;
        if (val != cur) {
            data[pos / 64] ^= 1ull << (pos & 63);
        }
    }

    int count() const {
        int ans = 0;
        for (auto it : data) {
            ans += __builtin_popcountll(it);
        }
        return ans;
    }

    void operator&=(const Bitset& b) {
        for (int i = 0; i < b.data.size(); i++) {
            data[i] &= b.data[i];
        }
    }
};

int bitset_and_count(const Bitset& a, const Bitset& b) {
    int ans = 0;
    for (int i = 0; i < a.data.size(); i++) {
        ans += __builtin_popcountll(a.data[i] & b.data[i]);
    }
    return ans;
}

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
        in.g.clear();
        in.gr.clear();
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
    vector<Bitset> reachability_table;
    vector<int> topsort;

    Checker() {}
    Checker(const Input& in) : in(in) {
        used.resize(in.n);
        build_reachability_table();
        topsort = in.get_topsort(false);
    }
    bool is_correct_region(const vector<int>& marked, const vector<int>& region) {
        Bitset region_bitset(in.n);
        for (auto v : region) region_bitset.set(v, 1);
        int cnt = 0;
        for (auto blocked : region) {
            if (marked[blocked]) {
                ++cnt;
                // cerr << blocked << ' ' << bitset_and_count(reachability_table[blocked], region_bitset) << endl;
                if (bitset_and_count(reachability_table[blocked], region_bitset)) {
                    // cerr << "[Error] There is path that contains not all marked verticies from some region";
                    return false;
                }
            }
        }
        if (cnt == 0) return false;
        return true;
    }
    bool is_correct_region_union(const vector<int>& marked, const vector<int>& region1, const vector<int>& region2) {
        Bitset region_bitset(in.n);
        for (auto v : region1) region_bitset.set(v, 1);
        for (auto v : region2) region_bitset.set(v, 1);
        int cnt = 0;
        for (auto blocked : region1) {
            if (marked[blocked]) {
                ++cnt;
                if (bitset_and_count(reachability_table[blocked], region_bitset)) {
                    // cerr << "[Error] There is path that contains not all marked verticies from some region";
                    return false;
                }
            }
        }
        for (auto v : region2) {
            if (marked[v]) {
                ++cnt;
                if (bitset_and_count(reachability_table[v], region_bitset)) {
                    // cerr << "[Error] There is path that contains not all marked verticies from some region";
                    return false;
                }
            }
        }
        if (cnt == 0) return false;
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
            Bitset region_bitset(in.n);
            for (const auto& region : out.regions) {
                bool has_marked = false;
                for (auto v : region) region_bitset.set(v, 1);
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
                        if (bitset_and_count(reachability_table[v], region_bitset)) {
                            cerr << "[Error] There is path that contains not all marked verticies from some region";
                            return false;
                        }
                    }
                }
                if (!has_marked) {
                    cerr << "[Error] Some region does not contain marked vertex" << endl;
                    return false;
                }
                for (auto v : region) region_bitset.set(v, 0);
            }
            for (int i = 0; i < in.n; i++) {
                if (all.count(i) == 0) {
                    cerr << "[Error] Vertex " << i + 1 << " does not belong to any region" << endl;
                    return false;
                }
            }
        }

        for (const auto& region : out.regions) {
            pair<int, int> cur = get_region_beta_pair(region);
            // cerr << cur.first << ' ' << cur.second << endl;
            if (1ll * cur.first * 10 < 9 * cur.second) {
                return false;
            }
        }
        return true;
    }
    int score(const Output& out, bool verbose = true) {
        if (!is_correct(out)) {
            return 0;
        }

        double min_beta = 1.0;
        for (const auto& region : out.regions) {
            pair<int, int> cur = get_region_beta_pair(region);
            // cerr << cur.first << ' ' << cur.second << endl;
            if (1ll * cur.first * 10 < 1ll * 9 * cur.second) {
                cerr << "[Error] There is region with beta < 0.9" << endl;
                return 0;
            }
            if (cur.first > cur.second) {
                cerr << "[Error] Beta" << cur.first << ' ' << cur.second << endl;
                for (auto v : region) cerr << v + 1 << ' ';
                cerr << endl;
                return 0;
            }
            min_beta = min(min_beta, 1.0 * cur.first / cur.second);
        }
        if (verbose) cerr << "[Info] Min beta = " << fixed << setprecision(8) << min_beta << endl;

        return out.regions.size();
    }

    void update(pair<int, int> &a, int b) {
        if ((b == a.first) || (b == a.second)) return;
        if (b < a.second) a.second = b;
        if (a.second < a.first) swap(a.first, a.second);
    }

    pair<int, int> get_region_beta_pair(const vector<int>& region) {
        vector<int> cur_w(in.n, 0);
        for (int v : region) cur_w[v] = in.w[v];
        vector<pair<pair<int, int>, int>> dp(in.n);
        for (int v : topsort) {
            dp[v] = {{1e9, 1e9}, 0};
            for (int to : in.g[v]) {
                update(dp[v].first, dp[to].first.first);
                update(dp[v].first, dp[to].first.second);
                update(dp[v].first, dp[to].second);
                dp[v].second = max(dp[v].second, dp[to].second);
            }
            if (in.g[v].size() == 0) dp[v] = {{0, 0}, 0};
            dp[v].first.first += cur_w[v];
            dp[v].first.second += cur_w[v];
            dp[v].second += cur_w[v];
        }
        if (dp[0].first.first == 0) {
            return {dp[0].first.second, dp[0].second};
        }
        return {dp[0].first.first, dp[0].second};
    }

    bool is_correct_region_beta(const vector<int>& region) {
        auto cur = get_region_beta_pair(region);
        return 1ll * cur.first * 10 >= 1ll * 9 * cur.second;
    }

    pair<int, int> get_region_union_beta_pair(const vector<int>& region1, const vector<int>& region2) {
        vector<int> cur_w(in.n, 0);
        for (int v : region1) cur_w[v] = in.w[v];
        for (int v : region2) cur_w[v] = in.w[v];
        vector<pair<pair<int, int>, int>> dp(in.n);
        for (int v : topsort) {
            dp[v] = {{1e9, 1e9}, 0};
            for (int to : in.g[v]) {
                update(dp[v].first, dp[to].first.first);
                update(dp[v].first, dp[to].first.second);
                update(dp[v].first, dp[to].second);
                dp[v].second = max(dp[v].second, dp[to].second);
            }
            if (in.g[v].size() == 0) dp[v] = {{0, 0}, 0};
            dp[v].first.first += cur_w[v];
            dp[v].first.second += cur_w[v];
            dp[v].second += cur_w[v];
        }
        if (dp[0].first.first == 0) {
            return {dp[0].first.second, dp[0].second};
        }
        return {dp[0].first.first, dp[0].second};
    }

private:
    vector<int> used;
    vector<int> used1;
    int timer = 0;

    void build_reachability_table() {
        reachability_table.clear();
        reachability_table.resize(in.n, Bitset(in.n));

        used1 = used;
        // for (int i = 1; i < in.n - 1; i++) reachability_table[i][0] = 1;
        // for (int i = 1; i < in.n - 1; i++) reachability_table[i][in.n - 1] = 1;
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
            ++timer;
            q.push(0);
            used1[0] = timer;
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                for (int to : in.g[v]) {
                    if (to == blocked || used1[to] == timer) continue;
                    used1[to] = timer;
                    q.push(to);
                }
            }
            for (int i = 1; i < in.n - 1; i++) {
                if (i == blocked) continue;
                if (used[i] == timer - 1 && used1[i] == timer) {
                    reachability_table[blocked].set(i, 1);
                }
            }
            if (used1[in.n - 1] == timer) {
                reachability_table[blocked].set(0, 1);
                reachability_table[blocked].set(in.n - 1, 1);
            }
        }
        // for (int i = 0; i < in.n; i++) {
        //     for (int j = 0; j < in.n; j++) {
        //         cerr << reachability_table[i][j];
        //     }
        //     cerr << endl;
        // }
    }
};

Input input_generator() {
    Input in;
    in.n = 100 + rand() % 400;
    for (int i = 0; i < in.n; i++) in.w.push_back(rand() % 100 + 1);
    in.k = 0;
    for (int i = rand() % in.n; i < in.n; i += rand() % (10 + rand() % 40) + 1) {
        in.k++;
        in.u.push_back(i);
    }
    in.g.resize(in.n);
    in.gr.resize(in.n);
    
    vector<bool> has_income(in.n, false);
    for (int i = in.n - 2; i > 0; i--) {
        vector<int> all(in.n - i - 1);
        iota(all.begin(), all.end(), i + 1);
        random_shuffle(all.begin(), all.end());
        for (int j = 0; j < min<int>(all.size(), 5000 / in.n); j++) {
            in.edges.push_back({i, all[j]});
            in.g[i].push_back(all[j]);
            in.gr[all[j]].push_back(i);
            has_income[all[j]] = true;
        }
    }
    for (int i = 1; i < in.n; i++) {
        if (!has_income[i]) {
            in.edges.push_back({0, i});
            in.g[0].push_back(i);
            in.gr[i].push_back(0);
        } 
    }
    in.m = in.edges.size();
    return in;
}
}  // namespace utils