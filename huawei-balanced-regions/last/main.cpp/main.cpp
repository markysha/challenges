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
}  // namespace utils#include <bits/stdc++.h>

using namespace std;

const int N = 5000;

class DSU {
private:
    enum class ActionType {
        SET_MARKED,
        UNITE
    };
    struct Action {
        ActionType type;
        pair<int, int> last;
    };

    vector<int> marked;
    vector<int> t;
    int n;
    vector<Action> history;
    int components;
public:

    vector<vector<int>> r;

    int get(int x) const {
        return t[x] == x ? x : get(t[x]);
    }

    void rollback() {
        auto cur = history.back();
        history.pop_back();
        if (cur.type == ActionType::SET_MARKED) {
            marked[cur.last.first] = cur.last.second;
        }
        if (cur.type == ActionType::UNITE) {
            int root = cur.last.first;
            int v = cur.last.second;
            t[v] = v;
            int last_size = r[root].size() - r[v].size();
            r[root].resize(last_size);
            components += 1;
        }
    }

    int time() const {
        return history.size();
    }

    void rollback(int time) {
        while (history.size() > time) rollback();
    }

    int get_components() const {
        return components;
    }

    void set_marked(int v, int value) {
        history.push_back({ActionType::SET_MARKED, {v, marked[v]}});
        marked[v] = value;
    }

    const vector<int>& get_marked() const {
        return marked;
    }

    inline int get_marked(int v) const {
        return marked[v];
    }

    int unite(int x, int y) {
        x = get(x);
        y = get(y);
        if (x == y) return 0;
        if (r[x].size() <= r[y].size()) {
            t[x] = y;
            // r[y].insert(r[y].end(), r[x].begin(), r[x].end());
            for (auto v: r[x]) r[y].push_back(v);
            history.push_back({ActionType::UNITE, {y, x}});
        } else {
            t[y] = x;
            // cerr << r[x].size() << ' ';
            // r[x].insert(r[x].end(), r[y].begin(), r[y].end());
            for (auto v: r[y]) r[x].push_back(v);
            // cerr << r[x].size() << endl;
            history.push_back({ActionType::UNITE, {x, y}});
        }
        components -= 1;
        return 1;
    }

    DSU(int n) : n(n) {
        components = n;
        t.resize(n);
        r.assign(n, {});
        marked.assign(n, 0);
        for (int i = 0; i < n; i++) t[i] = i, r[i] = {i};
    }

    DSU() {}
};

DSU dsu;
DSU best_dsu;
vector<int> used;
vector<int> used1;
vector<int> paired_list;
vector<int> paired;
int timer = 0;
Input in;
utils::Checker checker;

void merge_dependent() {
    for (int blocked = 1; blocked < in.n - 1; blocked++) {
        for (int i = blocked + 1; i < in.n - 1; i++) {
            if (dsu.get(i) == dsu.get(blocked)) continue;
            if (!checker.reachability_table[i][blocked] && !checker.reachability_table[blocked][i]) {
                dsu.set_marked(i, dsu.get_marked(i) | 2);
                dsu.set_marked(blocked, dsu.get_marked(blocked) | 2);
                dsu.unite(i, blocked);
            }
        }
        if (!checker.reachability_table[blocked][in.n - 1]) {
            dsu.unite(blocked, 0);
        }
    }
}

void dsu_to_out(DSU dsu, Output& out) {
    if (out.regions.size() > 0 && dsu.get_components() >= out.regions.size()) {
        return;
    }

    out.regions.clear();
    out.v.clear();

    out.regions.resize(in.n);
    for (int i = 0; i < in.n; i++) {
        out.regions[dsu.get(i)].push_back(i);
    }
    sort(out.regions.begin(), out.regions.end(), [](const vector<int> &a, const vector<int> &b) {
        return a.size() > b.size();
    });
    while (out.regions.back().size() == 0) out.regions.pop_back();
    for (const auto &region : out.regions) {
        if (region.size() == 1) {
            dsu.set_marked(region[0], dsu.get_marked(region[0]) | 2);
        }
    }
    //
    for (int i = 0; i < in.n; i++) {
        if (dsu.get_marked(i) == 2) {
            out.v.push_back(i);
        }
    }
}

void to_first_marked(int v, vector<int> &comp) {
    used[v] = timer;
    // if (comp.size() > 5) return;
    comp.push_back(v);
    for (int to : in.g[v]) {
        if (used[to] == timer) continue;
        // if (dsu.get_marked(to) & 1) continue;
        to_first_marked(to, comp);
    }
}

void clever_search(double TIME) {
    for (int i = 0; i < in.n; i++) {
        if (dsu.get_marked(i) == 0) continue;
        ++timer;
        for (auto x : dsu.r[dsu.get(i)]) {
            used[x] = timer;
        }

        vector<int> comp;
        // ++timer;
        to_first_marked(i, comp);
        int checkpoint = dsu.time();
        Region region;
        for (int i = 0; i < comp.size(); i++) {
            for (auto x : dsu.r[dsu.get(comp[i])]) {
                if (x == i) continue;
                int mark = dsu.get_marked(x);
                if (mark & 2) {
                    dsu.set_marked(x, mark & 1);
                }
            }
        }
        // for (auto v : region) cerr << v << endl;
        // cerr << endl;
        for (auto v: comp) {
            int root = dsu.get(v);
            for (auto x : dsu.r[root]) region.push_back(x);
        }
        sort(region.begin(), region.end());
        region.erase(unique(region.begin(), region.end()), region.end());

        if (checker.is_correct_region(dsu.get_marked(), region) && checker.is_correct_region_beta(region)) {
            // cerr << i << ' ' << region.size() << endl;
            // for (auto x : region) cerr << x << ' ';
            // cerr << endl;

            for (int j = 1; j < region.size(); j++) {
                dsu.unite(region[0], region[j]);
            }
        } else {
            dsu.rollback(checkpoint);
        }
    }
}

void random_iter() {
    vector<int> path;
    {
        int v = 0;
        while (true) {
            path.push_back(v);
            if (v == in.n - 1) break;
            v = in.g[v][rand() % in.g[v].size()];
        }
    }
    int len = path.size();
    bool f = false;
    while (len >= 2 && !f) {
        for (int l = 0; l + len <= path.size(); l++) {
            int r = l + len - 1;
            vector<int> subpath(path.begin() + l, path.begin() + r + 1);
            vector<int> region;
            for (auto v: subpath) {
                int root = dsu.get(v);
                for (auto x : dsu.r[root]) region.push_back(x);
            }
            sort(region.begin(), region.end());
            region.erase(unique(region.begin(), region.end()), region.end());
            
            int marked = 0;
            for (auto x : region) marked += dsu.get_marked(x) > 0;
            if (marked == 0) continue;
 
            int checkpoint = dsu.time();
 
            int v1 = -1, v2 = -1;
 
            for (auto x : region) {
                int mark = dsu.get_marked(x);
                if (mark & 2) {
                    v2 = v2 == -1 ? x : (rand() % 2 ? v2 : x);
                    dsu.set_marked(x, mark & 1);
                }
                if (mark & 1) {
                    v1 = v1 == -1 ? x : (rand() % 2 ? v1 : x);
                }
            }
 
            if (v1 == -1) {
                dsu.set_marked(v2, dsu.get_marked(v2) | 2);
            }

            if (checker.is_correct_region(dsu.get_marked(), region) && checker.is_correct_region_beta(region)) {
                f = true;
                for (auto it : region) {
                    dsu.unite(region[0], it);
                }
                break;
            }
            dsu.rollback(checkpoint);
        }
        len /= 2;
    }
}

void random_search(double TIME) {
    while (1.0 * clock() / CLOCKS_PER_SEC < TIME) {
        random_iter();
    }
}

void solver1(double TIME) {
    vector<int> nodes;
    vector<int> comps;
    for (int i = 0; i < in.n; i++) {
        if (dsu.get(i) == i) {
            nodes.emplace_back(i);
        } else {
            comps.emplace_back(dsu.get(i));
        }
    }
    sort(comps.begin(), comps.end());
    comps.erase(unique(comps.begin(), comps.end()), comps.end());
            

    vector<int> cur_w = in.w;
    sort(nodes.begin(), nodes.end(), [&cur_w](int a, int b) {
        return cur_w[a] < cur_w[b];
    });

    for (auto node: nodes) {
        int best_comp_pos = -1;
        double best_beta = 0;

        for (int i = 0; i < comps.size(); i++) {
            int comp = comps[i];
            if (!checker.is_correct_region_union(dsu.get_marked(), dsu.r[comp], {node})) continue;
            auto beta_pair = checker.get_region_union_beta_pair(dsu.r[comp], {node});
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) continue;
            double cur_beta = 1.0 * beta_pair.first / beta_pair.second;
            if (cur_beta > best_beta) {
                best_beta = cur_beta;
                best_comp_pos = i;
            }
        }

        if (best_comp_pos != -1) {
            dsu.unite(node, comps[best_comp_pos]);
            comps[best_comp_pos] = dsu.get(node);
        }
    }
}

void prepare(int v, int u) {
    int marked_v = 0;
    int marked_u = 0;
    for (auto x : dsu.r[v]) marked_v += dsu.get_marked(x) > 0;
    for (auto x : dsu.r[u]) marked_u += dsu.get_marked(x) > 0;
 
    const auto& region1 = dsu.r[v];
    const auto& region2 = dsu.r[u];
 
    int v1 = -1, v2 = -1;
 
    for (auto x : region1) {
        int mark = dsu.get_marked(x);
        if (mark & 2) {
            v2 = (v2 == -1 ? x : (rand() % 2 ? v2 : x));
            dsu.set_marked(x, mark & 1);
        }
        if (mark & 1) {
            v1 = (v1 == -1 ? x : (rand() % 2 ? v1 : x));
        }
    }
    for (auto x : region2) {
        int mark = dsu.get_marked(x);
        if (mark & 2) {
            v2 = (v2 == -1 ? x : (rand() % 2 ? v2 : x));
            dsu.set_marked(x, mark & 1);
        }
        if (mark & 1) {
            v1 = (v1 == -1 ? x : (rand() % 2 ? v1 : x));
        }
    }
 
    if (v1 == -1) {
        dsu.set_marked(v2, dsu.get_marked(v2) | 2);
    }
}

void solver2(double TIME) {
    vector<int> comps;
    for (int i = 0; i < in.n; i++) {
        if (dsu.get(i) == i) {
        } else {
            comps.emplace_back(dsu.get(i));
        }
    }
    sort(comps.begin(), comps.end());
    comps.erase(unique(comps.begin(), comps.end()), comps.end());
            

    vector<int> comp_size(in.n);
    for (int i = 0; i < in.n; i++) comp_size[dsu.get(i)] += in.w[i];

    sort(comps.begin(), comps.end(), [&comp_size](int a, int b) {
        return comp_size[a] < comp_size[b];
    });

    for (int i = 0; i < comps.size(); i++) {
        int comp = comps[i];
        int best_comp_pos = -1;
        double best_beta = 0;

        for (int j = i + 1; j < comps.size(); j++) {
            int comp2 = comps[j];

            assert(dsu.get(comp) != dsu.get(comp2));
            assert(dsu.get(comp) == comp);
            assert(dsu.get(comp2) == comp2);

            int checkpoint = dsu.time();
            prepare(comp, comp2);

            if (!checker.is_correct_region_union(dsu.get_marked(), dsu.r[comp], dsu.r[comp2])) {
                dsu.rollback(checkpoint);
                continue;
            }
            auto beta_pair = checker.get_region_union_beta_pair(dsu.r[comp], dsu.r[comp2]);
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) {
                dsu.rollback(checkpoint);
                continue;
            }
            double cur_beta = 1.0 * beta_pair.first / beta_pair.second;
            if (cur_beta > best_beta) {
                best_beta = cur_beta;
                best_comp_pos = j;
            }

            dsu.rollback(checkpoint);
        }

        if (best_comp_pos != -1) {
            prepare(comp, comps[best_comp_pos]);
            dsu.unite(comp, comps[best_comp_pos]);
            comps[best_comp_pos] = dsu.get(comp);
        }
    }
}

void solve(double TIME) {
    checker = utils::Checker(in);

    paired_list = {0, in.n - 1};
    paired.assign(in.n, 0);
    used.assign(in.n, 0);

    dsu = DSU(in.n);
    dsu.unite(0, in.n - 1);
    for (int x : in.u) dsu.set_marked(x, 1);
    dsu.set_marked(0, dsu.get_marked(0) | 2);

    merge_dependent();
    best_dsu = dsu;
    // connect_not_marked_to_paired();
    // clever_search(TIME);
    // random_search(TIME);
    solver1(TIME);
    solver2(TIME);
    best_dsu = dsu;

    Output out;
    dsu_to_out(dsu, out);
    // if (!checker.is_correct(out)) {
    //     assert(0);
    // }
    cout << out << endl;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    // {
    //     // cerr << "kek" << endl;
    //     // cout << 50 << endl;

    //     for (int i = 0; i < 100; i++) {
    //         cerr << i << endl;
    //         in = utils::input_generator();
            
    //         solve();
    //         // cout << in << endl;
    //     }
    //     return 0;
    // }
    const double MAX_TIME = 19;

    int t;
    cin >> t;
    for (int i = 0; i < t; i++) {
        in.init_test();
        cin >> in;
        solve(MAX_TIME / t * (i + 1));
    }
    return 0;
}
