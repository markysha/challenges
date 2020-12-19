#include <bits/stdc++.h>

#include "../lib/common.cpp"  // INSERT_COMMON_HERE
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
