#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> g;
vector<pair<int, int>> edges;

vector<int> split(const string& line) {
    vector<int> ans;
    for (int r = 0, l = 0; r < line.size(); r++) {
        if (r + 1 == line.size() || line[r + 1] == ' ') {
            ans.emplace_back(stoi(line.substr(l, r - l + 1)));
            ++r;
            l = r + 1;
        }
    }
    return ans;
}

int main() {
    int n, m;
    cin >> n >> m;
    string line;
    getline(cin, line);
    int real_n = 0;
    for (int i = 1; i <= n; i++) {
        getline(cin, line);
        auto nodes = split(line);
        for (int node : nodes) {
            // --node;
            int x = min(i, node);
            int y = max(i, node);
            if (x == y) {
                continue;
            }
            real_n = max(real_n, y);
            edges.emplace_back(x, y);
        }
    }
    sort(edges.begin(), edges.end());
    edges.erase(unique(edges.begin(), edges.end()), edges.end());
    g.resize(real_n + 1);
    for (const auto& edge : edges) {
        g[edge.first].emplace_back(edge.second);
        g[edge.second].emplace_back(edge.first);
    }

    cout << real_n << ' ' << edges.size() << endl;
    for (int v = 1; v <= real_n; v++) {
        bool f = false;
        for (auto u : g[v]) {
            if (f) {
                cout << ' ';
            }
            f = true;
            cout << u;
        }
        cout << endl;
    }
    // for (auto edge : edges) {
    //     pair<int, int> r_edge{edge.second, edge.first};
    //     auto it = binary_search(edges.begin(), edges.end(), r_edge);
    //     if (it == edges.end()) {
    //         assert("Graph should be ")
    //     }
    // }
}