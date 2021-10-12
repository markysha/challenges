#ifndef _GLIBCXX_NO_ASSERT

#include <cassert>

#endif

#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#if __cplusplus >= 201103L

#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>

#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201103L

#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#endif

#include <string_view>

constexpr int SFL = 200;
constexpr int GFL = 100;

using namespace std;

namespace utils {};  // namespace utils

struct Edge {
    int id;
    int group_id;
    int v;
    int u;
    int distance;
    int capacity;
};

struct Constrain {
    int node_id;
    pair<int, int> edge_ids;
};

struct Flow {
    int id;
    int source;
    int target;
    int flow;
};

struct Input {
    int n;
    int m;
    vector<Edge> edges;
    vector<Flow> flows;
    vector<Constrain> constrains;

    void init_test() {

    }

    friend ostream &operator<<(ostream &stream, const Input &in) {
        return stream;
    }

    friend istream &operator>>(istream &stream, Input &in) {
        int constrained;
        int flow_count;
        stream >> in.n >> in.m >> constrained >> flow_count;
        in.edges.resize(in.m);
        for (auto &edge : in.edges) {
            stream >> edge.id >> edge.group_id >> edge.v >> edge.u >> edge.distance >> edge.capacity;
        }
        in.constrains.resize(constrained);
        for (auto &constrain: in.constrains) {
            stream >> constrain.node_id >> constrain.edge_ids.first >> constrain.edge_ids.second;
        }
        in.flows.resize(flow_count);
        for (auto &flow: in.flows) {
            stream >> flow.id >> flow.source >> flow.target >> flow.flow;
        }
        return stream;
    }
};

struct Output {
    vector<pair<int, vector<int>>> paths;

    Output() = default;

    void init_test() {
    }

    friend ostream &operator<<(ostream &stream, const Output &out) {
        stream << out.paths.size() << endl;
        for (const auto &path: out.paths) {
            stream << path.first << ' ';
            for (auto it: path.second) {
                stream << it << ' ';
            }
            stream << endl;
        }
        return stream;
    }

    friend istream &operator>>(istream &stream, Output &out) {

        return stream;
    }
};

namespace utils {

    struct Checker {
        Checker() {}

        Checker(const Input &in) {

        }

        int score(const Output &out, bool verbose = true) {
            return 0;
        }

    private:

    };

}  // namespace utils
using namespace std;

//const bool DEBUG = false;
const int MAX_EDGE_ID = 1<<14;
const int MAX_GROUP_ID = 4501;

mt19937 rnd(0);

struct PairHasher {
    size_t operator()(const pair<int, int> &x) const {
        return x.first * MAX_EDGE_ID + x.second;
    }
} blocked_hacher;

struct OrderedEdge {
    int id;
    int v;
    int group_id;
};

struct State {
    vector<Flow> flow_by_id;
    vector<OrderedEdge> ordered_edges;
    vector<pair<int, int>> ordered_edges_ids;
    vector<pair<int, int>> input_in_ordered_edges;
    vector<int> edge_capacity;
    vector<int> node_flow_size;
    vector<int> group_id_flow_size;

//    vector<pair<int, int>> ordered_constrains;
//    vector<pair<int, int>> ordered_constrains_ids;
//    vector<int> constrains_used;
//    int constains_timer = 0;

    bitset<MAX_EDGE_ID*MAX_EDGE_ID> blocked;
//    unordered_set<pair<int, int>, PairHasher> constrains;
} state;

void init_state(const Input &in) {
    state.flow_by_id = in.flows;

    state.ordered_edges.reserve(in.m * 2);
    for (int i = 0; i < in.m; i++) {
        const auto &e = in.edges[i];
        state.ordered_edges.push_back({e.id, e.v, e.group_id});
        state.ordered_edges.push_back({e.id, e.u, e.group_id});
    }
    sort(state.ordered_edges.begin(), state.ordered_edges.end(), [&in](const OrderedEdge &a, const OrderedEdge &b) {
        const auto &ae = in.edges[a.id];
        const auto &be = in.edges[b.id];
        int au = a.v == ae.u ? ae.v : ae.u;
        int bu = b.v == be.u ? be.v : be.u;
        if (au == bu) return ae.capacity < be.capacity;
        return au < bu;
    });

    state.ordered_edges_ids.resize(in.n);
    state.input_in_ordered_edges.resize(in.m);
    for (int i = 0; i < in.m * 2; i++) {
        const auto& e = in.edges[state.ordered_edges[i].id];
        if (e.v == state.ordered_edges[i].v) {
            state.input_in_ordered_edges[e.id].first = i;
        } else {
            state.input_in_ordered_edges[e.id].second = i;
        }

        int prid = i == 0 ? -1 : state.ordered_edges[i - 1].id;
        int pru =
                i == 0 ? -1 : (in.edges[prid].u == state.ordered_edges[i - 1].v ? in.edges[prid].v : in.edges[prid].u);

        int id = state.ordered_edges[i].id;
        int u = (in.edges[id].u == state.ordered_edges[i].v ? in.edges[id].v : in.edges[id].u);

        if (i == 0 || u != pru) {
            state.ordered_edges_ids[u].first = i;
        }
        state.ordered_edges_ids[u].second = i + 1;
    }

    state.edge_capacity.resize(in.m);
    for (int i = 0; i < in.m; i++) {
        state.edge_capacity[i] = in.edges[i].capacity;
    }

    state.node_flow_size.resize(in.n);

    state.group_id_flow_size.resize(MAX_GROUP_ID);

//    state.ordered_constrains.reserve(in.constrains.size() * 2);
//    for (size_t i = 0; i < in.constrains.size(); i++) {
//        const auto &c = in.constrains[i];
//        state.ordered_constrains.push_back(c.edge_ids);
//        state.ordered_constrains.push_back({c.edge_ids.second, c.edge_ids.first});
//    }
//    sort(state.ordered_constrains.begin(), state.ordered_constrains.end());
//    state.ordered_constrains_ids.resize(MAX_EDGE_ID + 1);
//    for (size_t i = 0; i < in.constrains.size() * 2; i++) {
//        int a = state.ordered_constrains[i].first;
//        int pra = i == 0 ? -1 : state.ordered_constrains[i - 1].first;
//
//        if (i == 0 || a != pra) {
//            state.ordered_constrains_ids[a].first = i;
//        }
//        state.ordered_constrains_ids[a].second = i + 1;
//    }
//
//    state.constrains_used.resize(MAX_EDGE_ID + 1);

//    state.constrains.max_load_factor(0.25);
//    state.constrains.reserve(in.constrains.size() * 3);
    for (const auto &constrain: in.constrains) {
        state.blocked[blocked_hacher(constrain.edge_ids)] = 1;
        state.blocked[blocked_hacher({constrain.edge_ids.second, constrain.edge_ids.first})] = 1;
    }
}

optional<vector<int>>
find_path(const Flow &flow, const Input &in, int limit = 1e9, int node_flow_limit = SFL, int group_flow_limit = GFL) {
    if (state.node_flow_size[flow.source] >= SFL) {
        return nullopt;
    }
    static vector<int> d;
    d.assign(in.n, 1e9);
//    vector<int> dc(in.n, 1e9);
    static vector<int> pr;
    pr.assign(in.n, MAX_EDGE_ID);
    d[flow.source] = 0;
    vector<int> q;
    size_t q_start = 0;
    q.push_back(flow.source);
    while (q_start < q.size()) {
        int v = q[q_start++];
        if (v == flow.target) break;
        if (d[v] > limit) break;

        int e_id = pr[v] == MAX_EDGE_ID ? 0 : state.ordered_edges[pr[v]].id;
//        state.constains_timer++;
//        for (int i = state.ordered_constrains_ids[e_id].first; i < state.ordered_constrains_ids[e_id].second; ++i) {
//            const auto& e = state.ordered_constrains[i];
//            state.constrains_used[e.second] = state.constains_timer;
//        }
        for (int i = state.ordered_edges_ids[v].first; i < state.ordered_edges_ids[v].second; i++) {
            const auto &e = state.ordered_edges[i];
            if (state.node_flow_size[e.v] >= node_flow_limit) continue;
            if (state.group_id_flow_size[e.group_id] >= group_flow_limit) continue;
            if (state.edge_capacity[e.id] < flow.flow) continue;
            if (state.blocked[blocked_hacher({e_id, e.id})]) continue;
            if (d[e.v] > d[v] + 1) {
                d[e.v] = d[v] + 1;
                q.push_back(e.v);
                pr[e.v] = i;
            }
        }
    }
    if (d[flow.target] > in.n) return nullopt;
    std::cerr << "Found path for " << flow.id << std::endl;
    vector<int> ans;
    int v = flow.target;
    while (true) {
        int e_id = state.ordered_edges[pr[v]].id;
        int to = in.edges[e_id].v == v ? in.edges[e_id].u : in.edges[e_id].v;
        ans.push_back(pr[v]);
        v = to;
        if (to == flow.source) break;
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

void update_state(const Flow &flow, const vector<int> &path, const Input &in) {
    ++state.node_flow_size[flow.source];
    ++state.node_flow_size[flow.target];
    for (int id: path) {
        int e_id = state.ordered_edges[id].id;
//        const auto& e = in.edges[e_id];
        state.edge_capacity[e_id] -= flow.flow;

//        auto ids = state.input_in_ordered_edges[e_id];
//        {
//            auto v = e.u;
//            int l = state.ordered_edges_ids[v].first;
//            int p = ids.first;
//            while (p > l && state.edge_capacity[state.ordered_edges[p].id] < state.edge_capacity[state.ordered_edges[p - 1].id]) {
//                swap(state.ordered_edges[p], state.ordered_edges[p - 1]);
//                --p;
//            }
//        }

        ++state.group_id_flow_size[in.edges[e_id].group_id];
    }
    for (size_t i = 0; i < path.size() - 1; i++) {
        ++state.node_flow_size[state.ordered_edges[path[i]].v];
    }
}

void add_state(const Flow &flow, const vector<int> &path, const Input &in, int add) {
    state.node_flow_size[flow.source] += add;
    state.node_flow_size[flow.target] += add;
    for (int id: path) {
        int e_id = state.ordered_edges[id].id;
//        const auto& e = in.edges[e_id];
        state.edge_capacity[e_id] -= flow.flow * add;

//        auto ids = state.input_in_ordered_edges[e_id];
//        {
//            auto v = e.u;
//            int l = state.ordered_edges_ids[v].first;
//            int p = ids.first;
//            while (p > l && state.edge_capacity[state.ordered_edges[p].id] < state.edge_capacity[state.ordered_edges[p - 1].id]) {
//                swap(state.ordered_edges[p], state.ordered_edges[p - 1]);
//                --p;
//            }
//        }

        state.group_id_flow_size[in.edges[e_id].group_id] += add;
    }
    for (size_t i = 0; i < path.size() - 1; i++) {
        state.node_flow_size[state.ordered_edges[path[i]].v] += add;
    }
}

void solve(Input in) {
    Output out;

    init_state(in);
    std::cerr << "State inited" << std::endl;

    sort(in.flows.begin(), in.flows.end(), [](const Flow &a, const Flow &b) {
        return a.flow < b.flow;
    });

    set<int> found;
    for (int j = 5; j < 9; j++) {
        for (int i = 0; i < 6; i++) {
            for (const auto &flow : in.flows) {
                if (found.count(flow.id)) continue;
                auto path_opt = find_path(flow, in, 1 << i, min(SFL, 1 << j), min(GFL, 1 << j));
                if (path_opt) {
//                    vector<int> path{};
//                    for (auto x: path_opt.value()) {
//                        path.push_back(state.ordered_edges[x].id);
//                    }

                    found.insert(flow.id);
                    update_state(flow, path_opt.value(), in);
                    out.paths.emplace_back(flow.id, std::move(path_opt.value()));
                }
            }
        }
    }
    while(1.0 * clock() / CLOCKS_PER_SEC < 1.85) {
        shuffle(out.paths.begin(), out.paths.end(), rnd);
        int cnt = 2;
        for (size_t i = out.paths.size() - 1; i >= out.paths.size() - cnt; --i) {
            add_state(state.flow_by_id[out.paths[i].first], out.paths[i].second, in, -1);
//            found.erase(out.paths[i].first);
        }
        Output cur;
        for (const auto &flow : in.flows) {
            if (found.count(flow.id)) continue;
            auto path_opt = find_path(flow, in, 16);
            if (path_opt) {
                found.insert(flow.id);
                add_state(flow, path_opt.value(), in, 1);
                cur.paths.emplace_back(flow.id, std::move(path_opt.value()));
            }
            if (cur.paths.size() > cnt) break;
        }
//        for (size_t i = out.paths.size() - 1; i >= out.paths.size() - cnt; --i) {
//            auto flow = state.flow_by_id[out.paths[i].first];
//            if (found.count(flow.id)) continue;
//            auto path_opt = find_path(flow, in, 16);
//            if (path_opt) {
//                found.insert(flow.id);
//                add_state(flow, path_opt.value(), in, 1);
//                cur.paths.emplace_back(flow.id, std::move(path_opt.value()));
//            }
//        }
        if (cur.paths.size() >= cnt) {
            for (int i = 0; i < cnt; i++) {
                found.erase(out.paths.back().first);
                out.paths.pop_back();
            }
            for (auto& it: cur.paths) {
                out.paths.push_back(it);
            }
        } else {
            for (const auto& it: cur.paths) {
                add_state(state.flow_by_id[it.first], it.second, in, -1);
                found.erase(it.first);
            }
            for (size_t i = out.paths.size() - 1; i >= out.paths.size() - cnt; --i) {
                add_state(state.flow_by_id[out.paths[i].first], out.paths[i].second, in, 1);
                found.insert(out.paths[i].first);
            }
        }
    }
    for (auto& it: out.paths) {
        vector<int> path{};
        for (auto x: it.second) {
            path.push_back(state.ordered_edges[x].id);
        }
        it.second = std::move(path);
    }
//    for (auto it : state.ordered_edges) {
//        std::cerr << it.id << ' ';
//    }
//    std::cerr << endl;
//    for (auto it : state.node_flow_size) {
//        cerr << it << ' ';
//    }
//    cerr << endl;
//    for (auto it : state.edge_capacity) {
//        cerr << it << ' ';
//    }
//    cerr << endl;
    assert(out.paths.size() > 0);
    cout << out;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    srand(time(0));

    Input in;
    cin >> in;
    solve(in);
    return 0;
}
