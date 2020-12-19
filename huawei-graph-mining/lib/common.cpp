#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;
using Point = pair<int, int>;

struct Edge {
    int u;
    int v;
};

struct ExtendedEdge {
    int u;
    int v;
    int weight;
    int id;
};

struct Score {
    double modularity;
    double regularization;
    double sum() {
        return modularity + regularization;
    }
    bool operator<(const Score& a) {
        return modularity + regularization < a.modularity + a.regularization;
    }
};

namespace utils {};  // namespace utils

struct Input {
    int n, m;
    vector<Edge> edges;
    vector<vector<pair<int, int>>> g;  // (to, id)

    friend ostream& operator<<(ostream& stream, Input& in) {
        for (const auto& edge : in.edges) {
            stream << edge.u << ' ' << edge.v << '\n';
        }

        return stream;
    }

    friend istream& operator>>(istream& stream, Input& in) {
        int a, b;
        int n = 0;
        while (stream >> a >> b) {
            in.edges.emplace_back(Edge{a, b});
            n = max(n, a);
            n = max(n, b);
        }

        in.n = n + 1;
        in.m = in.edges.size();

        in.g.clear();
        in.g.resize(in.n);
        for (int i = 0; i < in.m; ++i) {
            int u = in.edges[i].u;
            int v = in.edges[i].v;

            in.g[u].emplace_back(v, i);
            in.g[v].emplace_back(u, i);
        }

        return stream;
    }
};

struct Output {
    vector<vector<int>> components;

    Output() = default;

    void init_test() {
        components.clear();
    }

    friend ostream& operator<<(ostream& stream, const Output& out) {
        for (const auto& component : out.components) {
            for (auto id : component) {
                stream << id << ' ';
            }
            stream << '\n';
        }
        return stream;
    }
    friend istream& operator>>(istream& stream, Output& out) {
        out.components.clear();

        string line;
        while (getline(stream, line)) {
            stringstream line_stream(std::move(line));
            vector<int> component;
            int v;
            while (line_stream >> v) {
                component.emplace_back(v);
            }
            out.components.emplace_back(component);
        }
        return stream;
    }
};

struct DSU {
    DSU(int n) {
        parents.resize(n);
        iota(parents.begin(), parents.end(), 0);
        rk.resize(n, 0);
        components.resize(n);
        for (int i = 0; i < n; i++) {
            components[i].emplace_back(i);
        }
        roots = unordered_set<int>(parents.begin(), parents.end());
    }
    int find(int x) {
        if (parents[x] == x) {
            return x;
        }
        return parents[x] = find(parents[x]);
    }
    int find(int x) const {
        if (parents[x] == x) {
            return x;
        }
        return find(parents[x]);
    }
    void merge_components(int x, int y) {
        if (components[x].size() < components[y].size()) {
            swap(components[x], components[y]);
        }
        for (auto v : components[y]) {
            components[x].emplace_back(v);
        }
        components[y].clear();
        components[y].shrink_to_fit();
    }
    pair<int, int> connect(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return {-1, -1};
        }
        if (rk[y] < rk[x]) swap(x, y);
        parents[x] = y;
        rk[y] += rk[x];
        merge_components(y, x);
        roots.erase(x);
        return {y, x};
    }
    pair<int, int> connect_directed(int x, int y) {
        int x1 = x;
        int y1 = y;

        x = find(x);
        y = find(y);

        assert(x1 == x);
        assert(y1 == y);
        if (x == y) {
            assert(x != y);
            return {-1, -1};
        }
        parents[y] = x;
        rk[x] += rk[y];
        merge_components(x, y);
        roots.erase(y);
        return {x, y};
    }
    bool is_connected(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return true;
        }
        return false;
    }
    int components_number() {
        return roots.size();
    }
    vector<int> rk;
    vector<int> parents;
    unordered_set<int> roots;
    vector<vector<int>> components;
};

namespace utils {
void fails(string comment) {
    cerr << comment << endl;
    exit(1);
}

double sqr(double x) {
    return x * x;
}

bool check_valid(const Input& in, const Output& out) {
    unordered_map<int, int> cnt;
    for (int i = 0; i < in.n; i++) cnt[i]++;
    for (const auto& component : out.components) {
        for (auto v : component) {
            cnt[v]--;
        }
    }
    for (const auto& it : cnt) {
        if (it.second > 0) {
            fails("Not found vertex in output. First not found: " + to_string(it.first));
            return false;
        }
        if (it.second < 0) {
            fails("Not valid vertex in output. First not valid: " + to_string(it.first));
        }
    }
    return true;
}

tuple<vector<int>, vector<int>> get_edges_number_inside_outside_component(const Input& in, const Output& out) {
    vector<int> component_number(in.n);
    int timer = 0;
    for (const auto& component : out.components) {
        for (auto v : component) {
            component_number[v] = timer;
        }
        ++timer;
    }

    vector<int> edges_number_inside_component(timer);
    vector<int> edges_number_outside_component(timer);

    for (const auto& edge : in.edges) {
        int u = edge.u;
        int v = edge.v;
        if (component_number[v] == component_number[u]) {
            int cur_component_number = component_number[v];
            ++edges_number_inside_component[cur_component_number];
        } else {
            ++edges_number_outside_component[component_number[u]];
            ++edges_number_outside_component[component_number[v]];
        }
    }

    return {edges_number_inside_component, edges_number_outside_component};
}

double score_regularization(const Input& in, const Output& out) {
    double score = 0;

    vector<int> edges_number_inside_component, edges_number_outside_component;
    tie(edges_number_inside_component, edges_number_outside_component) = get_edges_number_inside_outside_component(in, out);

    int timer = 0;
    for (const auto& component : out.components) {
        int sz = component.size();
        double density = sz == 1 ? 1.0 : (double)edges_number_inside_component[timer] / (1ll * sz * (sz - 1) / 2);

        score += (double)density / out.components.size();

        ++timer;
        // cerr << score << endl;
    }

    score -= (double)(out.components.size()) / in.n;

    return score * 0.5;
}

double component_score_modularity(const Input& in, const vector<int>& component) {
    double score = 0;
    unordered_set<int> st(component.begin(), component.end());

    int edges_number_inside_component = 0;
    int edges_number_outside_component = 0;

    for (int v : component) {
        for (const auto& it : in.g[v]) {
            int to = it.first;
            if (st.count(to)) {
                ++edges_number_inside_component;
            } else {
                ++edges_number_outside_component;
            }
        }
    }

    edges_number_inside_component /= 2;

    score += (double)edges_number_inside_component / in.m;
    score -= sqr((double)(edges_number_inside_component * 2 + edges_number_outside_component) / in.m / 2);

    return score;
}

double score_modularity(const Input& in, const Output& out) {
    if (!check_valid(in, out)) {
        return 0;
    }
    double score = 0;

    vector<int> edges_number_inside_component, edges_number_outside_component;
    tie(edges_number_inside_component, edges_number_outside_component) = get_edges_number_inside_outside_component(in, out);

    int timer = 0;
    for (const auto& component : out.components) {
        // score += (double)edges_number_inside_component[timer] / in.m;
        // score -= sqr((double)(edges_number_inside_component[timer] * 2 + edges_number_outside_component[timer]) / in.m / 2);
        score += (double)edges_number_inside_component[timer] / in.m;
        score -= sqr((double)(edges_number_inside_component[timer] * 2 + edges_number_outside_component[timer]) / in.m / 2);
        ++timer;
    }

    return score;
}

Score score(const Input& in, const Output& out) {
    if (!check_valid(in, out)) {
        return {0, 0};
    }
    return {score_modularity(in, out), score_regularization(in, out)};
}

namespace {
vector<bool> used;
int timer;
vector<int> tin, fup;
vector<int> bridges;

void dfs(const Input& in, int v, int p = -1) {
    used[v] = true;
    tin[v] = fup[v] = timer++;
    for (auto pr : in.g[v]) {
        auto to = pr.first;
        auto id = pr.second;
        if (to == p) {
            continue;
        }
        if (used[to]) {
            fup[v] = min(fup[v], tin[to]);
        } else {
            dfs(in, to, v);
            fup[v] = min(fup[v], fup[to]);
            if (fup[to] > tin[v]) {
                bridges.push_back(id);
            }
        }
    }
}
}  // namespace

vector<int> find_bridges(const Input& in) {
    timer = 0;
    used.resize(in.n);
    tin.resize(in.n);
    fup.resize(in.n);
    bridges.clear();

    for (int i = 0; i < in.n; ++i) {
        used[i] = false;
        tin[i] = 0;
        fup[i] = 0;
    }

    for (int i = 0; i < in.n; ++i) {
        if (!used[i]) {
            dfs(in, i);
        }
    }
    return bridges;
}

}  // namespace utils