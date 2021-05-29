#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
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
    // solver2(TIME);
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