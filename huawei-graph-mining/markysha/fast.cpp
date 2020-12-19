#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
using namespace std;

const double EPS = 1e-8;

mt19937 rnd(43);

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
vector<pair<int, int>> randomized(const vector<vector<ExtendedEdge>>& g, const DSU& dsu, vector<int> verties) {
    ++timer;
    shuffle(verties.begin(), verties.end(), rnd);
    vector<pair<int, int>> ans;
    for (auto u : verties) {
        if (used[u] == timer) continue;
        int v = -1;
        int cnt = 0;
        for (const auto& edge : g[u]) {
            int cur_v = dsu.find(edge.v);
            if (cur_v == u) continue;
            if (used[cur_v] == timer) {
                continue;
            }
            ++cnt;
        }
        if (cnt == 0) continue;
        cnt = min({rnd() % cnt}) + 1;
        for (const auto& edge : g[u]) {
            int cur_v = dsu.find(edge.v);
            if (cur_v == u) continue;
            if (used[cur_v] == timer) {
                continue;
            }
            if (--cnt == 0) {
                v = cur_v;
            }
        }
        if (v != -1) {
            ans.emplace_back(u, v);
            used[v] = timer;
            used[u] = timer;
        }
    }
    return ans;
}
vector<pair<int, int>> randomized_weighted(const vector<vector<ExtendedEdge>>& g, const DSU& dsu, vector<int> verties) {
    ++timer;
    shuffle(verties.begin(), verties.end(), rnd);
    vector<pair<int, int>> ans;
    for (auto u : verties) {
        if (used[u] == timer) continue;
        int v = -1;
        int rk = 0;
        for (const auto& edge : g[u]) {
            int cur_v = dsu.find(edge.v);
            if (cur_v == u) continue;
            if (used[cur_v] == timer) {
                continue;
            }
            if (dsu.rk[cur_v] > rk) {
                rk = dsu.rk[cur_v];
                v = cur_v;
            }
        }
        if (v != -1) {
            ans.emplace_back(u, v);
            used[v] = timer;
            used[u] = timer;
        }
    }
    return ans;
}
vector<pair<int, int>> randomized_best_intersection(const vector<vector<ExtendedEdge>>& g, const DSU& dsu, vector<int> verties) {
    ++timer;
    shuffle(verties.begin(), verties.end(), rnd);
    vector<pair<int, int>> ans;
    for (auto u : verties) {
        if (used[u] == timer) continue;
        int v = -1;
        long long intersection_size = -1;
        for (const auto& edge : g[u]) {
            int cur_v = edge.v;
            if (cur_v == u) continue;
            if (used[cur_v] == timer) {
                continue;
            }
            long long cur_intersection_size = 1ll * edge.weight * edge.weight + edges_lists_intersection(g[u], g[cur_v]);
            if (cur_intersection_size > intersection_size) {
                intersection_size = cur_intersection_size;
                v = cur_v;
            }
        }
        if (v != -1) {
            ans.emplace_back(u, v);
            used[v] = timer;
            used[u] = timer;
        }
    }
    return ans;
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
    // vector<ExtendedEdge> remain;
    // auto it = g[a].begin();
    // for (auto& edge : g[b]) {
    //     while (it != g[a].end() && it->v < edge.v) {
    //         ++it;
    //     }
    //     if (it != g[a].end() && it->v == edge.v) {
    //         it->weight += edge.weight;
    //     } else {
    //         remain.push_back(edge);
    //     }
    // }
    // g[b].resize(remain.size() + g[a].size());
    // merge(
    //     g[a].begin(),
    //     g[a].end(),
    //     remain.begin(),
    //     remain.end(),
    //     g[b].begin(),
    //     [](const ExtendedEdge& a, const ExtendedEdge& b) {
    //         return a.v < b.v;
    //     });

    // swap(g[a], g[b]);
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
    for (int i = 0; i < in.n; i++) {
        component_modularity[i] = utils::component_score_modularity(in, dsu.components[i]);
    }

    int timer = 350;
    while (timer--) {
        vector<int> verties(dsu.roots.begin(), dsu.roots.end());
        sort(verties.begin(), verties.end());

        for (auto v : verties) {
            for (auto& edge : g[v]) {
                edge.v = dsu.find(edge.v);
                edge.u = dsu.find(edge.u);
            }
            sort(g[v].begin(), g[v].end(), [](const auto& a, const auto& b) {
                return a.v < b.v;
            });
            fit(g, v);
        }
        {
            Output cur_out;
            cur_out.components.reserve(dsu.components_number());
            for (auto v : verties) {
                cur_out.components.push_back(dsu.components[v]);
            }
            Score cur_out_score = utils::score(in, cur_out);

            cerr << dsu.components_number() << ' ';
            cerr << cur_out_score.modularity << ' ';
            cerr << cur_out_score.regularization << ' ';
            cerr << cur_out_score.modularity + cur_out_score.regularization << endl;

            if (out_score < cur_out_score) {
                out = move(cur_out);
                out_score = cur_out_score;
            }
        }
        if (dsu.components_number() < 10) break;

        auto cur_matching = matching::randomized_best_intersection(g, dsu, verties);
        cerr << cur_matching.size() << endl;
        bool increased = false;
        for (const auto& it : cur_matching) {
            int a = dsu.find(it.first);
            int b = dsu.find(it.second);

            vector<int> cur(dsu.components[a].size() + dsu.components[b].size());
            copy(dsu.components[a].begin(), dsu.components[a].end(), cur.begin());
            copy(dsu.components[b].begin(), dsu.components[b].end(), cur.begin() + dsu.components[a].size());

            double cur_modularity = utils::component_score_modularity(in, cur);

            if (cur_modularity > component_modularity[a] + component_modularity[b]) {
                auto cur_roots = dsu.connect(it.first, it.second);
                merge_verties(g, cur_roots.first, cur_roots.second);
                component_modularity[cur_roots.first] = cur_modularity;
                increased = true;
            }
        }
        if (!increased) break;
    }

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
