#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
using namespace std;

mt19937 rnd(time(0));

double sqr(double x) {
    return x * x;
}

struct Graph {
    vector<vector<ExtendedEdge>> g;
    vector<int> component_id;
    vector<unordered_set<int>> components_sets;
    vector<int> component_rk;
    vector<int> component_edges_number_inside;
};

namespace enhances {
// Score score_swap_components(int from_component, int to_component, int v, const Graph& graph) {
//     Score delta{0, 0};
// }
unordered_map<int, int> calculate_component_edges_by_id(int v, const Graph& graph) {
    unordered_map<int, int> component_edges_by_id;
    for (const auto& edge : graph.g[v]) {
        int u = edge.v;

        int a = graph.component_id[u];

        ++component_edges_by_id[a];
    }
    return component_edges_by_id;
}
Score score_move_vertex_from_component(int v, pair<int, int>& components_ids, const Graph& graph, const Input& in) {
    Score best{-1, -1};
    auto component_edges_by_id = calculate_component_edges_by_id(v, graph);
    auto density = [](int ein, int c) {
        if (c == 1) return 1.0;
        return 2.0 * ein / c / (c - 1);
    };
    for (const auto& edge : graph.g[v]) {
        int u = edge.v;

        int a = graph.component_id[v];
        int b = graph.component_id[u];

        if (a == b) continue;
        if (graph.components_sets[a].size() == 1) continue;

        int x = component_edges_by_id[a];
        int y = component_edges_by_id[b];
        int sz = graph.g[v].size();

        Score cur{0, 0};

        cur.modularity = 1.0 * (y - x) / (in.m);
        cur.modularity -= (2 * sqr(1.0 * sz / (2 * in.m)));
        cur.modularity += 2.0 * (graph.component_rk[a] - graph.component_rk[b]) / (2 * in.m) * (1.0 * sz / (2 * in.m));

        cur.regularization -= density(graph.component_edges_number_inside[a], graph.components_sets[a].size());
        cur.regularization += density(graph.component_edges_number_inside[a] - x, graph.components_sets[a].size() - 1);
        cur.regularization -= density(graph.component_edges_number_inside[b], graph.components_sets[b].size());
        cur.regularization += density(graph.component_edges_number_inside[b] + y, graph.components_sets[b].size() + 1);
        cur.regularization /= graph.components_sets.size();
        cur.regularization *= 0.5;

        if (cur.modularity + cur.regularization > best.modularity + best.regularization) {
            best = cur;
            components_ids = {a, b};
        }
    }
    return best;
}
void move_vertex(int v, pair<int, int> component_ids, Graph& graph) {
    int from = component_ids.first;
    int to = component_ids.second;
    assert(from != to);
    assert(graph.components_sets[from].size() > 1);
    auto component_edges_by_id = calculate_component_edges_by_id(v, graph);

    graph.component_id[v] = to;
    graph.components_sets[from].erase(v);
    graph.components_sets[to].insert(v);
    graph.component_rk[from] -= graph.g[v].size();
    graph.component_rk[to] += graph.g[v].size();
    graph.component_edges_number_inside[from] -= component_edges_by_id[from];
    graph.component_edges_number_inside[to] += component_edges_by_id[to];
}
}  // namespace enhances

Output enhance(Input in, Output out, Score score) {
    Graph graph;

    graph.g.resize(in.n);

    for (int i = 0; i < in.n; i++) {
        for (const auto& edge : in.g[i]) {
            graph.g[i].push_back({i, edge.first, 1, edge.second});
        }
        sort(graph.g[i].begin(), graph.g[i].end(), [](const auto& a, const auto& b) {
            return a.v < b.v;
        });
    }

    graph.component_id.resize(in.n, 0);
    graph.component_rk.resize(in.n, 0);
    graph.components_sets.resize(out.components.size());
    graph.component_edges_number_inside.resize(out.components.size(), 0);

    for (int i = 0; i < out.components.size(); i++) {
        const auto& component = out.components[i];
        graph.components_sets[i].reserve(component.size());
        for (auto v : component) {
            graph.component_id[v] = i;
            graph.component_rk[i] += graph.g[v].size();
            graph.components_sets[i].insert(v);
        }
    }

    for (const auto& edge : in.edges) {
        int u = edge.u;
        int v = edge.v;
        if (graph.component_id[v] == graph.component_id[u]) {
            int cur_component_number = graph.component_id[v];
            ++graph.component_edges_number_inside[cur_component_number];
        } else {
        }
    }

    Score out_score = utils::score(in, out);

    vector<int> ids(in.n);
    iota(ids.begin(), ids.end(), 0);

    int timer = 20;
    auto getA = []() {
        return (rnd() % 1000) / 1000.0;
    };
    long double T = 0.00001000 * (rnd() % 100);
    long double K = 0.7 + (1.0 * (rnd() % 30) / 100);
    while (--timer > 0) {
        std::cerr << "timer = " << timer << endl;
        // std::cerr << "timer = " << timer << endl;
        bool enhanced = false;
        int tick = ids.size() / 30;
        shuffle(ids.begin(), ids.end(), rnd);
        for (auto v : ids) {
            pair<int, int> components_ids = {-1, -1};
            Score cur = enhances::score_move_vertex_from_component(v, components_ids, graph, in);
            if (components_ids.first == -1) continue;
            double delta = cur.modularity + cur.regularization;
            if (delta > 0 || exp((1.0 * delta) / T) >= getA()) {
                enhanced = true;
                enhances::move_vertex(v, components_ids, graph);
                out_score.modularity += cur.modularity;
                out_score.regularization += cur.regularization;
            }
            if (--tick == 0) {
                T *= K;
                tick = ids.size() / 30;
            }
        }
        if (!enhanced) break;
        T *= K;

        if (true) {
            // Output cur_out;
            // for (const auto& component_set : graph.components_sets) {
            //     vector<int> component;
            //     for (auto v : component_set) component.emplace_back(v);
            //     cur_out.components.emplace_back(component);
            // }
            // Score cur_score = utils::score(in, cur_out);
            std::cerr << "modularity     " << out_score.modularity << endl;
            // std::cerr << "modularity check " << cur_score.modularity << endl;
            std::cerr << "regularization " << out_score.regularization << endl;
            std::cerr << "score          " << out_score.modularity + out_score.regularization << endl;
            // std::cerr << "regularization check " << cur_score.regularization << endl;
        }
    }

    out.components.clear();
    for (const auto& component_set : graph.components_sets) {
        vector<int> component;
        for (auto v : component_set) component.emplace_back(v);
        // assert(component.size() > 0);
        if (component.size()) out.components.emplace_back(component);
    }

    return out;
}

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(0);

    string inputfile(argv[1]);
    string outputfile(argv[2]);
    ifstream input(inputfile);
    ifstream output(outputfile);

    Input in;
    input >> in;
    Output out;
    output >> out;

    sort(out.components.begin(), out.components.end(), [](const vector<int>& a, const vector<int>& b) {
        return a.size() > b.size();
    });
    vector<int> ones;
    // shuffle(out.components.end() - 10000, out.components.end(), rnd());
    // int cnt = 2000;
    while (out.components.back().size() == 1) {
        ones.push_back(out.components.back()[0]);
        out.components.pop_back();
    }
    shuffle(ones.begin(), ones.end(), rnd);
    cerr << ones.size() << endl;
    while (ones.size() > 0) {
        vector<int> cur = {ones.back()};
        ones.pop_back();
        out.components.push_back(cur);
    }
    if (ones.size()) out.components.push_back(ones);
    // cerr << cnt << endl;

    Score score = utils::score(in, out);
    std::cerr << "INITIAL:" << endl;
    std::cerr << fixed << setprecision(8);
    std::cerr << "modularity: " << score.modularity << endl;
    std::cerr << "regularization: " << score.regularization << endl;
    std::cerr << "SCORE = " << score.modularity + score.regularization << endl;

    for (int i = 0; i < 5; i++) {
        Output cur = out;
        Score cur_score = score;
        cur = enhance(in, cur, cur_score);
        cur_score = utils::score(in, cur);
        if (score < cur_score) {
            out = cur;
            score = cur_score;
        }
    }

    // for (int i = 0; i < 10; i++) {
    //     Output cur;
    //     cur.components = out.components;

    //     int n = cur.components.size();
    //     for (int i = 0; i < cur.components.size(); i++) {
    //         shuffle(cur.components[i].begin(), cur.components[i].end(), rnd);
    //         if (i > 0 && cur.components.size() > 10) {
    //             int cnt = rnd() % (cur.components.size() / 10) + 1;
    //             while (cnt-- > 0) {
    //                 int v = cur.components[i].back();
    //                 cur.components[i - 1].push_back(v);
    //                 cur.components[i].pop_back();
    //             }
    //         }
    //     }

    //     Score cur_score = utils::score(in, cur);
    //     cur = enhance(in, cur, cur_score);

    //     if (score < cur_score) {
    //         score = cur_score;
    //         out = cur;
    //     }
    // }

    std::cerr << "FINAL:" << endl;
    std::cerr << fixed << setprecision(8);
    std::cerr << "modularity: " << score.modularity << endl;
    std::cerr << "regularization: " << score.regularization << endl;
    std::cerr << "SCORE = " << score.modularity + score.regularization << endl;

    std::cout << out;
}
