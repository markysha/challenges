#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;
using Point = pair<int, int>;

struct Edge {
    int u;
    int v;
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
    }
    int find(int x) {
        if (parents[x] == x) {
            return x;
        }
        return parents[x] = find(parents[x]);
    }
    bool connect(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return false;
        }
        if (rand() % 2) {
            swap(x, y);
        }
        parents[x] = y;
        return true;
    }
    bool is_connected(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return true;
        }
        return false;
    }
    vector<int> parents;
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
        double density = sz == 1 ? 1 : (double)edges_number_inside_component[timer] / (1ll * sz * (sz - 1) / 2);

        score += (double)density / out.components.size();

        ++timer;
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
    double score = 0;

    vector<int> edges_number_inside_component, edges_number_outside_component;
    tie(edges_number_inside_component, edges_number_outside_component) = get_edges_number_inside_outside_component(in, out);

    int timer = 0;
    for (const auto& component : out.components) {
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

}  // namespace utils#include <bits/stdc++.h>

using namespace std;

const double EPS = 1e-8;

vector<int> calc_distances_from(const Input& in, int start) {
    vector<int> d(in.n, 1e9);
    queue<int> q;
    q.push(start);
    d[start] = 0;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (const auto& it : in.g[v]) {
            int to = it.first;
            if (d[to] > d[v] + 1) {
                q.push(to);
                d[to] = d[v] + 1;
            }
        }
    }
    return d;
}

vector<int> get_verticies_in_distance(const Input& in, int start, int length) {
    vector<int> d(in.n, 1e9);
    queue<int> q;
    vector<int> ans;
    q.push(start);
    d[start] = 0;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        ans.push_back(v);
        for (const auto& it : in.g[v]) {
            int to = it.first;
            if (d[to] > d[v] + 1) {
                d[to] = d[v] + 1;
                if (d[to] > length) continue;
                q.push(to);
            }
        }
    }
    return ans;
}

void save_dfs_path(const Input& in, int v, vector<int>& path, vector<int>& used) {
    used[v] = true;
    path.emplace_back(v);
    for (const auto& it : in.g[v]) {
        int to = it.first;
        if (!used[to]) {
            save_dfs_path(in, to, path, used);
        }
    }
}

void save_bfs_path(const Input& in, int v, vector<int>& path, vector<int>& used) {
    used[v] = true;
    queue<int> q;
    q.push(v);
    while (!q.empty()) {
        int v = q.front();
        path.push_back(v);
        q.pop();
        for (const auto& it : in.g[v]) {
            int to = it.first;
            if (!used[to]) {
                used[to] = true;
                q.push(to);
            }
        }
    }
}

void solve() {
    Input in;
    Output out;

    cin >> in;

    out.components = {{0, 1, 2, 3}, {4, 5, 6, 7}};

    auto d = calc_distances_from(in, 0);
    d = calc_distances_from(in, max_element(d.begin(), d.end()) - d.begin());
    cerr << "~ diameter: " << (*max_element(d.begin(), d.end())) << endl;

    vector<int> all(in.n);
    iota(all.begin(), all.end(), 0);
    sort(all.begin(), all.end(), [&](int a, int b) {
        return in.g[a].size() > in.g[b].size();
    });
    vector<int> p_all(in.n);
    for (int i = 0; i < in.n; i++) {
        p_all[all[i]] = i;
    }

    for (int i = 0; i < in.n; i++) {
        sort(in.g[i].begin(), in.g[i].end(), [&](pair<int, int> a, pair<int, int> b) {
            return p_all[a.first] < p_all[b.first];
        });
    }

    vector<int> path;
    vector<int> used(in.n);
    // save_dfs_path(in, all[0], path, used);
    save_bfs_path(in, all[0], path, used);

    vector<double> dp(in.n, -1e9);
    vector<int> pr(in.n);

    for (int i = 0; i < in.n; i++) {
        int min_size = sqrt(in.n);
        vector<int> component;
        int p = i + 1;
        for (int cur_size = min_size; cur_size <= min_size + min_size; cur_size += min_size / 3) {
            int j = max(0, i - cur_size);
            while (j > 0 && dp[j - 1] < EPS) --j;
            while (p - 1 >= j) {
                --p;
                component.emplace_back(path[p]);
            }
            double cur = utils::component_score_modularity(in, component);
            if ((j == 0 ? 0 : dp[j - 1]) + cur > dp[i]) {
                dp[i] = (j == 0 ? 0 : dp[j - 1]) + cur;
                pr[i] = j - 1;
            }
        }
        if (i % 100 == 0) cerr << i << ' ' << dp[i] << endl;
    }
    out.components.clear();
    for (int i = in.n - 1; i >= 0; i = pr[i]) {
        int j = pr[i];
        out.components.push_back({});
        while (j < i) {
            ++j;
            out.components.back().emplace_back(path[j]);
        }
    }

    cout << out;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    solve();
    return 0;
}
