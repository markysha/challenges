#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
using namespace std;

const double EPS = 1e-8;

mt19937 rnd(time(0));

namespace matching {
int timer = 0;
vector<int> used(200000, 0);
bool operator<(const ExtendedEdge& a, const ExtendedEdge& b) {
    return a.v < b.v;
}

long long edges_lists_intersection(const vector<ExtendedEdge>& ga, const vector<ExtendedEdge>& gb) {
    auto first1 = ga.begin();
    auto last1 = ga.end();
    auto first2 = gb.begin();
    auto last2 = gb.end();

    long long result = 0;
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else {
            result += min(first1->weight, first2->weight);
            ++first1;
            ++first2;
        }
    }
    return result;
}

vector<pair<int, int>> edges_lists_intersection_vector(const vector<ExtendedEdge>& ga, const vector<ExtendedEdge>& gb) {
    auto first1 = ga.begin();
    auto last1 = ga.end();
    auto first2 = gb.begin();
    auto last2 = gb.end();

    vector<pair<int, int>> result;
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else {
            result.push_back({first1->v, min(first1->weight, first2->weight)});
            ++first1;
            ++first2;
        }
    }
    return result;
}
}  // namespace matching

void fit(vector<vector<ExtendedEdge>>& g, int v) {
    int p = 0;
    for (int i = 1; i < g[v].size(); i++) {
        if (g[v][i].v == g[v][p].v) {
            g[v][p].weight += g[v][i].weight;
        } else {
            g[v][++p] = g[v][i];
        }
    }
    g[v].resize(p + 1);
    for (int i = 0; i < g[v].size(); i++) {
        if (g[v][i].v == v) {
            for (int j = i + 1; j < g[v].size(); j++) {
                g[v][j - 1] = g[v][j];
            }
            g[v].resize(p);
        }
    }
}

void merge_verties(vector<vector<ExtendedEdge>>& g, int a, int b) {
    if (g[a].size() < g[b].size()) {
        swap(g[a], g[b]);
    }
    for (const auto& edge : g[b]) {
        g[a].emplace_back(edge);
    }

    g[b].clear();
    g[b].shrink_to_fit();
}

void recalc(int v, DSU& dsu, vector<vector<ExtendedEdge>>& g) {
    for (auto& edge : g[v]) {
        edge.v = dsu.find(edge.v);
        edge.u = dsu.find(edge.u);
    }
    sort(g[v].begin(), g[v].end(), [](const auto& a, const auto& b) {
        return a.v < b.v;
    });
    fit(g, v);
}

int cnt_edges_from_to(int v, int u, vector<vector<ExtendedEdge>>& g) {
    int l = 0, r = (int)g[v].size() - 1;
    int p = 0;
    while (l <= r) {
        int mid = (l + r) / 2;
        if (u <= g[v][mid].v) {
            p = mid;
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    return (p < g[v].size() && g[v][p].v == u ? g[v][p].weight : 0);
}

pair<int, double> find_best(int v, DSU& dsu, vector<vector<ExtendedEdge>>& g, const Input& in) {
    pair<int, double> ans = {-1, 0};
    for (auto& edge : g[v]) {
        int u = edge.v;
        // fit(g, u);
        assert(u != v);

        int common = edge.weight;
        double delta = 2.0 * (1.0 * common / (in.m * 2) - 1.0 * (1.0 * dsu.rk[v] / (in.m * 2)) * (1.0 * dsu.rk[u] / (in.m * 2)));

        if (delta > ans.second) {
            ans = {u, delta};
        }
    }
    return ans;
}

void combine(int a, int b, DSU& dsu, vector<vector<ExtendedEdge>>& g, map<int, int>& Tr, set<pair<int, int>>& T) {
    dsu.connect_directed(a, b);
    fit(g, b);
    merge_verties(g, a, b);
    auto common = matching::edges_lists_intersection_vector(g[a], g[b]);
    for (auto [v, weight] : common) {
        T.erase(T.find({Tr[v], v}));
        --Tr[v];
        T.insert({Tr[v], v});
        assert(v != a && v != b);
    }
    if (T.find({Tr[b], b}) != T.end())
        T.erase(T.find({Tr[b], b}));

    Tr[a] = g[a].size();
    T.insert({Tr[a], a});
}

Output multilevel(Input in) {
    Output out;
    Score out_score({0, 0});
    vector<vector<ExtendedEdge>> g(in.n);
    DSU dsu(in.n);

    for (int i = 0; i < in.n; i++) {
        for (const auto& edge : in.g[i]) {
            g[i].push_back({i, edge.first, 1, edge.second});
        }
        sort(g[i].begin(), g[i].end(), [](const auto& a, const auto& b) {
            return a.v < b.v;
        });
        dsu.rk[i] = g[i].size();
    }

    vector<double> component_modularity(in.n);
    double graph_modularity = 0;
    for (int i = 0; i < in.n; i++) {
        component_modularity[i] = utils::component_score_modularity(in, dsu.components[i]);
        graph_modularity += component_modularity[i];
    }

    map<int, int> Tr;
    set<pair<int, int>> T;
    for (int i = 0; i < in.n; i++) {
        T.insert({g[i].size(), i});
        Tr[i] = g[i].size();
    }
    int timer = 0;

    while (!T.empty()) {
        // vector<int> P;
        // while (!T.empty() && T.begin()->first == 1) {
        //     int v = T.begin()->second;
        //     T.erase(T.begin());

        //     recalc(v, dsu, g);

        //     if (g[v].size() == 0) continue;
        //     if (g[v].size() == 1) {
        //         int u = g[v][0].v;
        //         combine(v, u, dsu, g, Tr, T);
        //     } else {
        //         Tr[v] = g[v].size();
        //         T.insert({g[v].size(), v});
        //     }
        // }

        // if (T.empty()) break;

        int v = dsu.find(T.begin()->second);
        if (Tr[v] != T.begin()->first || v != T.begin()->second) {
            T.erase(T.begin());
            continue;
        }
        T.erase(T.begin());

        recalc(v, dsu, g);

        pair<int, double> u = find_best(v, dsu, g, in);

        if (u.first != -1) {
            combine(v, u.first, dsu, g, Tr, T);
            graph_modularity += u.second;
            component_modularity[v] += u.second + component_modularity[u.first];
            component_modularity[u.first] = 0;
        }
        // if (1) {
        //     cerr << "delta = " << u.second << " (" << v << ' ' << u.first << ")" << endl;
        // }

        if (++timer % 10000 == 0 || (timer >= 180000 && timer % 300 == 0)) {
            if (timer % 1000 == 0) {
                cerr << "timer = " << timer << endl;
                cerr << "graph_modularity = " << graph_modularity << endl;
                cerr << "components.size() = " << dsu.roots.size() << endl;
            }
            if (timer >= 180000) {
                Output cur;
                for (auto v : dsu.roots) {
                    cur.components.push_back(dsu.components[v]);
                }
                Score cur_score = utils::score(in, cur);

                if (timer % 300 == 0) {
                    cerr << "modularity = " << cur_score.modularity << " "
                         << "regularization = " << cur_score.regularization << endl;
                }

                if (out_score < cur_score) {
                    out_score = cur_score;
                    out = cur;
                }
            }
        }
        // for (auto v : dsu.roots) {
        //     for (auto u : dsu.components[v]) {
        //         cerr << u << ' ';
        //     }
        //     cerr << endl;
        // }
        // cerr << endl;
    }

    // for (auto v : dsu.roots) {
    //     out.components.push_back(dsu.components[v]);
    // }

    {
        vector<int> components_sizes;
        for (const auto& component : out.components) {
            components_sizes.push_back(component.size());
        }
        sort(components_sizes.rbegin(), components_sizes.rend());
        for (double percentile = 0; percentile < 100; percentile += 3) {
            cerr << components_sizes[components_sizes.size() * percentile / 100] << ' ';
        }
        cerr << endl;
    }
    return out;
}

void solve() {
    Input in;
    cin >> in;

    Output out = multilevel(in);
    cout << out;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    solve();
    return 0;
}
