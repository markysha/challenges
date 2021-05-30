 #include <bits/stdc++.h>

 #include "../lib/common.cpp" // INSERT_COMMON_HERE
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
 
    const vector<int>& get_t() {
        return t;
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
 
    void set_marked(const vector<int>& new_marked) {
        marked = new_marked;
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
                // if (dsu.get_marked(blocked) % 2 == 0 && dsu.get_marked(i) % 2 == 0) {
                //     if (rand() % 2) continue;
                // }
                dsu.set_marked(i, dsu.get_marked(i) | 2);
                dsu.set_marked(blocked, dsu.get_marked(blocked) | 2);
                dsu.unite(i, blocked);
            }
        }
        if (!checker.reachability_table[blocked][in.n - 1]) {
            dsu.unite(blocked, 0);
            dsu.set_marked(blocked, dsu.get_marked(blocked) | 2);
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
 
 
void solver11(double TIME, bool limited = false) {
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
    
    vector<int> comp_size(in.n);
    for (int i = 0; i < comps.size(); i++) {
        comp_size[comps[i]] = dsu.r[comps[i]].size();
    }
 
    vector<int> cur_w = in.w;
    vector<int> g_size(in.n);
    for (int i = 0; i < in.n; i++) g_size[i] = in.g[i].size();
    sort(nodes.begin(), nodes.end(), [&cur_w](int a, int b) {
        return cur_w[a] < cur_w[b];
    });
    sort(comps.begin(), comps.end(), [&comp_size](int a, int b) {
        return comp_size[a] < comp_size[b];
    });
 
    double REMAIN = (TIME - 1.0 * clock() / CLOCKS_PER_SEC) * 0.6;
    double START = 1.0 * clock() / CLOCKS_PER_SEC;
 
    for (int i = 0; i < comps.size(); i++) {
        int comp = comps[i];
        
        sort(nodes.begin(), nodes.end(), [&cur_w](int a, int b) {
            return cur_w[a] < cur_w[b];
        });
        vector<int> best_nodes;
        int best_cnt_good_nodes = 0;
 
        for (int t = 0; t < 9; t++) {
            comp = comps[i];
            // if (1.0 * clock() / CLOCKS_PER_SEC - START > REMAIN) break;
 
            int cnt_good_nodes = 0;
            int checkpoint = dsu.time();
            int cl = 0, cr = 0;
            int cnt = 0;
            for (auto node: nodes) {
                if (!checker.is_correct_region_node_union(dsu.get_marked(), dsu.r[comp], node)) {
                    // if (++cnt == 10) break;
                    continue;
                }
                auto beta_pair = checker.get_region_union_beta_pair(dsu.r[comp], {node});
                if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) {
                    // if (++cnt == 10) break;
                    continue;
                }
                cnt = 0;
 
                dsu.unite(node, comp);
                comp = dsu.get(node);
                ++cnt_good_nodes;
            }
            if (cnt_good_nodes >= best_cnt_good_nodes) {
                best_cnt_good_nodes = cnt_good_nodes;
                best_nodes = nodes;
            } else {
                reverse(nodes.begin() + cl, nodes.begin() + cr + 1);    
            }
            dsu.rollback(checkpoint);
            cl = rand() % nodes.size();
            cr = rand() % nodes.size();
            if (cl > cr) swap(cl, cr);
            // reverse(nodes.begin() + cl, nodes.begin() + cr + 1);
            random_shuffle(nodes.begin(), nodes.end());
            // if (t == 0) {
            //     sort(nodes.begin(), nodes.end(), [&cur_w](int a, int b) {
            //         return cur_w[a] > cur_w[b];
            //     });
            // }
            // if (t == 1) {
            //     sort(nodes.begin(), nodes.end(), [&g_size](int a, int b) {
            //         return g_size[a] < g_size[b];
            //     });
            // }
            // if (t == 2) {
            //     sort(nodes.begin(), nodes.end(), [&g_size](int a, int b) {
            //         return g_size[a] > g_size[b];
            //     });
            // }
        }
 
        vector<int> new_nodes;
        for (auto node: best_nodes) {
            if (!checker.is_correct_region_node_union(dsu.get_marked(), dsu.r[comp], node)) {
                new_nodes.push_back(node);
                continue;
            }
            auto beta_pair = checker.get_region_union_beta_pair(dsu.r[comp], {node});
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) {
                new_nodes.push_back(node);
                continue;
            }
 
            dsu.unite(node, comp);
            comp = dsu.get(node);
        }
 
        comps[i] = comp;
        nodes = new_nodes;
    }
}
 
void solver1(double TIME, bool limited = false) {
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
    
    vector<int> cnt(comps.size());
 
    vector<int> cur_w = in.w;
    sort(nodes.begin(), nodes.end(), [&cur_w](int a, int b) {
        return cur_w[a] < cur_w[b];
    });
    // random_shuffle(nodes.begin(), nodes.end());
 
 
    
    for (auto node: nodes) {
        int best_comp_pos = -1;
        double best_beta = -1e9;
 
        for (int i = 0; i < comps.size(); i++) {
            int comp = comps[i];
            if (!checker.is_correct_region_node_union(dsu.get_marked(), dsu.r[comp], node)) continue;
            // auto beta_pair_comp = checker.get_region_beta_pair(dsu.r[comp]);
            auto beta_pair = checker.get_region_union_beta_pair(dsu.r[comp], {node});
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) continue;
            double cur_beta = 1.0 * beta_pair.first / beta_pair.second;
            // if (limited && cur_beta < 0.95) continue;
            // double diff = 1.0 * beta_pair_comp.first / beta_pair_comp.second - cur_beta;
            cur_beta += 1.0 / dsu.r[comp].size();
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
 
void prepare(int v) {
    const auto& region = dsu.r[v];
 
    for (auto x : region) {
        int mark = dsu.get_marked(x);
        if (mark & 2) {
            dsu.set_marked(x, mark & 1);
        }
    }
}
 
void prepare(int v, int u) {
    int marked_v = 0;
    int marked_u = 0;
    for (auto x : dsu.r[v]) marked_v += dsu.get_marked(x) > 0;
    for (auto x : dsu.r[u]) marked_u += dsu.get_marked(x) > 0;
    if(0)assert(marked_u + marked_v >= 0);
 
    const auto& region1 = dsu.r[v];
    const auto& region2 = dsu.r[u];
 
    int v1 = -1, v2 = -1;
 
    for (auto x : region1) {
        int mark = dsu.get_marked(x);
        if (mark & 2) {
            v2 = (v2 == -1 ? x : (rand() % 2 ? v2 : x));
            if (rand() % 100) dsu.set_marked(x, mark & 1);
        }
        if (mark & 1) {
            v1 = (v1 == -1 ? x : (rand() % 2 ? v1 : x));
        }
    }
    for (auto x : region2) {
        int mark = dsu.get_marked(x);
        if (mark & 2) {
            v2 = (v2 == -1 ? x : (rand() % 2 ? v2 : x));
            if (rand() % 100) dsu.set_marked(x, mark & 1);
        }
        if (mark & 1) {
            v1 = (v1 == -1 ? x : (rand() % 2 ? v1 : x));
        }
    }
 
    if (v1 == -1) {
        if(0)assert(v2 != -1);
        dsu.set_marked(v2, dsu.get_marked(v2) | 2);
    }
}
 
void solver2(double TIME) {
    vector<int> comps;
    for (int i = 0; i < in.n; i++) {
        // if (dsu.get(i) == dsu.get(0) || dsu.get(i) == dsu.get(in.n - 1)) continue;
        if (dsu.get(i) == i) {
        } else {
            comps.emplace_back(dsu.get(i));
        }
    }
    sort(comps.begin(), comps.end());
    comps.erase(unique(comps.begin(), comps.end()), comps.end());
            
 
    vector<int> comp_size(in.n);
    for (int i = 0; i < in.n; i++) comp_size[dsu.get(i)] += 1;
 
    sort(comps.begin(), comps.end(), [&comp_size](int a, int b) {
        return comp_size[a] > comp_size[b];
    });
 
    int timer = 50;//
 
    for (int i = 0; i < comps.size(); i++) {
        int i_checkpoint = dsu.time();
        
        int comp = comps[i];
        if (comp == -1) continue;
        // int best_comp_pos = -1;
        // double best_beta = 0;
 
        vector<int> region = dsu.r[comp];
        vector<int> merge_list = {};
 
        for (int j = i + 1; j < comps.size(); j++) {
            int comp2 = comps[j];
            if (comp2 == -1) continue;
 
            // vector<int> marked = dsu.get_marked();
            if(0)assert(dsu.get(comp) != dsu.get(comp2));
            if(0)assert(dsu.get(comp) == comp);
            if(0)assert(dsu.get(comp2) == comp2);
 
            int checkpoint = dsu.time();
            if (merge_list.empty()) {
                prepare(comp, comp2);
            } else {
                prepare(comp2);
            }
 
            if (!checker.is_correct_region_union(dsu.get_marked(), region, dsu.r[comp2])) {
                dsu.rollback(checkpoint);
                // dsu.set_marked(marked);
                continue;
            }
            auto beta_pair = checker.get_region_union_beta_pair(region, dsu.r[comp2]);
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) {
                dsu.rollback(checkpoint);
                // dsu.set_marked(marked);
                continue;
            }
            // double cur_beta = 1.0 * beta_pair.first / beta_pair.second;
            // if (cur_beta > best_beta) {
            //     best_beta = cur_beta;
            //     best_comp_pos = j;
            // }
 
            // dsu.rollback(checkpoint);
 
            for (auto v : dsu.r[comp2]) {
                region.push_back(v);
                merge_list.push_back(j);
            }
            // dsu.set_marked(marked);
        }
        for (int j = i + 1; j < comps.size(); j++) {
            int comp2 = comps[j];
            if (comp2 == -1) continue;
 
            // vector<int> marked = dsu.get_marked();
            if(0)assert(dsu.get(comp) != dsu.get(comp2));
            if(0)assert(dsu.get(comp) == comp);
            if(0)assert(dsu.get(comp2) == comp2);
 
            int checkpoint = dsu.time();
            if (merge_list.empty()) {
                prepare(comp, comp2);
            } else {
                prepare(comp2);
            }
 
            if (!checker.is_correct_region_union(dsu.get_marked(), region, dsu.r[comp2])) {
                dsu.rollback(checkpoint);
                // dsu.set_marked(marked);
                continue;
            }
            auto beta_pair = checker.get_region_union_beta_pair(region, dsu.r[comp2]);
            if (1ll * beta_pair.first * 10 < 1ll * beta_pair.second * 9) {
                dsu.rollback(checkpoint);
                // dsu.set_marked(marked);
                continue;
            }
            // double cur_beta = 1.0 * beta_pair.first / beta_pair.second;
            // if (cur_beta > best_beta) {
            //     best_beta = cur_beta;
            //     best_comp_pos = j;
            // }
 
            // dsu.rollback(checkpoint);
 
            for (auto v : dsu.r[comp2]) {
                region.push_back(v);
                merge_list.push_back(j);
            }
            // dsu.set_marked(marked);
        }
 
        if (merge_list.size() > 0) {
            for (auto it : merge_list) {
                dsu.unite(comp, comps[it]);
            }
 
            Output out;
            dsu_to_out(dsu, out);
            if (!checker.is_correct(out)) {
                dsu.rollback(i_checkpoint);
                continue;
            }
            
            for (auto it : merge_list) {
                comps[it] = -1;
            }
            // comps[best_comp_pos] = dsu.get(comp);
            // if (--timer == 0) break;
        }
    }
}
 
void fork_solve1(Output &out, double TIME) {
    solver1(TIME);
    solver1(TIME);
    solver1(TIME);
    solver2(TIME);
    solver1(TIME);
    dsu_to_out(dsu, out);
}
 
void fork_solve2(Output &out, double TIME) {
    solver11(TIME);
    // clever_search(TIME);
    solver2(TIME);
    solver1(TIME);
    // solver1(TIME);
    dsu_to_out(dsu, out);
}
 
void solve(double TIME) {
    checker = utils::Checker(in);
 
    paired_list = {0, in.n - 1};
    paired.assign(in.n, 0);
    used.assign(in.n, 0);
    timer = 0;
 
    dsu = DSU(in.n);
    dsu.unite(0, in.n - 1);
    for (int x : in.u) dsu.set_marked(x, 1);
    dsu.set_marked(0, dsu.get_marked(0) | 2); 
    dsu.set_marked(in.n - 1, dsu.get_marked(in.n - 1) | 2); 
 
    merge_dependent();
    // connect_not_marked_to_paired();
    // clever_search(TIME);
    // random_search(TIME);
    Output out;
 
    // best_dsu = dsu;
    // dsu = best_dsu;
    if (dsu.get_components() > in.n / 2) {
        fork_solve2(out, TIME);
    } else {
        fork_solve1(out, TIME);
    }
    
    // if (!checker.is_correct(out)) {
    //     if(0)assert(0);
    // }
    cout << out << endl;
}
 
int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    srand(time(0));
    // DSU dsu(5);
    // int checkpoint = dsu.time();
    // for (auto it : dsu.get_t()) {
    //     cout << it << ' ';
    // }
    // cout << endl;
    // dsu.unite(0, 1);
    // dsu.unite(2, 3);
    // dsu.unite(3, 4);
    
    // for (auto it : dsu.get_t()) {
    //     cout << it << ' ';
    // }
    // cout << endl;
    // dsu.rollback(checkpoint);
 
    // for (auto it : dsu.get_t()) {
    //     cout << it << ' ';
    // }
    // cout << endl;
    // return 0;
 
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