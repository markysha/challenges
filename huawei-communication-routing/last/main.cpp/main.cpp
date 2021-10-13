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

constexpr unsigned char SFL = 200;
constexpr unsigned char GFL = 100;

using namespace std;

namespace utils {};  // namespace utils

struct Edge {
    short id;
    short group_id;
    short v;
    short u;
    int distance;
    int capacity;
};

struct Constrain {
    short node_id;
    pair<short, short> edge_ids;
};

struct Flow {
    short id;
    short source;
    short target;
    int flow;
};

struct Input {
    short n;
    short m;
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
const short MAX_EDGE_ID = 1 << 14;
const short MAX_GROUP_ID = 4501;

mt19937 rnd(3);

struct PairHasher {
    size_t operator()(int x, short y) const {
        return (x << 14) + y;
    }
} blocked_hacher;

struct OrderedEdge {
    short id;
    short v;
    short group_id;
};

struct State {
    vector<Flow> flow_by_id;
    vector<OrderedEdge> ordered_edges;
    vector<pair<short, short>> ordered_edges_ids;
    vector<pair<short, short>> input_in_ordered_edges;
    vector<int> edge_capacity;
    vector<unsigned char> node_flow_size;
    vector<unsigned char> group_id_flow_size;
    vector<bool> is_node_blocked;
    bitset<MAX_EDGE_ID * MAX_EDGE_ID> blocked;
} state;

Input in;

void init_state() {
    state.flow_by_id = in.flows;

    state.ordered_edges.clear();
    state.ordered_edges.reserve(in.m * 2);
    for (int i = 0; i < in.m; i++) {
        const auto &e = in.edges[i];
        state.ordered_edges.push_back({e.id, e.v, e.group_id});
        state.ordered_edges.push_back({e.id, e.u, e.group_id});
    }
    sort(state.ordered_edges.begin(), state.ordered_edges.end(), [](const OrderedEdge &a, const OrderedEdge &b) {
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
        const auto &e = in.edges[state.ordered_edges[i].id];
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

    state.is_node_blocked.resize(in.n);

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
        state.blocked[blocked_hacher(constrain.edge_ids.first, constrain.edge_ids.second)] = 1;
        state.blocked[blocked_hacher(constrain.edge_ids.second, constrain.edge_ids.first)] = 1;
    }
}

inline optional<vector<int>>
find_path(const Flow &flow, short limit = 30000, unsigned char node_flow_limit = SFL, unsigned char group_flow_limit = GFL) {
    if (state.node_flow_size[flow.source] >= node_flow_limit) {
        return nullopt;
    }
    static int used_timer = 0;
    used_timer++;
    static vector<int> used(in.n);
//    static vector<short> d(in.n);
    static vector<short> pr(in.n);

//    d[flow.source] = 0;
    pr[flow.source] = 0;
    used[flow.source] = used_timer;
    vector<short> q;
    size_t q_start = 0;
    q.push_back(flow.source);

    int d = 0;
    size_t next_d = 1;
    while (q_start < q.size()) {
        if (q_start == next_d) {
            ++d;
            next_d = q.size();
        }
        short v = q[q_start++];
        if (v == flow.target) break;
        if (d > limit) break;

        int e_id = state.ordered_edges[pr[v]].id;

        for (int i = state.ordered_edges_ids[v].first; i < state.ordered_edges_ids[v].second; i++) {
            const auto &e = state.ordered_edges[i];
            if (state.node_flow_size[e.v] >= node_flow_limit ||
                state.group_id_flow_size[e.group_id] >= group_flow_limit ||
                state.edge_capacity[e.id] < flow.flow ||
                state.blocked[blocked_hacher(e_id, e.id)] ||
                (state.is_node_blocked[e.v] && e.v != flow.target) ||
                used[e.v] == used_timer)
                continue;

            used[e.v] = used_timer;
//            d[e.v] = d[v] + 1;
            q.push_back(e.v);
            pr[e.v] = i;
        }
    }
    if (used[flow.target] != used_timer) return nullopt;
//    std::cerr << "Found path for " << flow.id << std::endl;
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

//optional<vector<int>>
//find_path2(const Flow &flow, int limit = 1e9, int node_flow_limit = SFL, int group_flow_limit = GFL) {
//    if (state.node_flow_size[flow.source] >= node_flow_limit || state.node_flow_size[flow.target] >= node_flow_limit) {
//        return nullopt;
//    }
//
//    static vector<pair<int, int>> d;
//    static vector<int> pr;
//    d.assign(in.n, {1e9, 0});
//    pr.assign(in.n, MAX_EDGE_ID);
//
//    d[flow.source] = {0, 0};
//    d[flow.target] = {0, 1};
//    vector<pair<int, int>> q;
//    size_t q_start = 0;
//    q.push_back({flow.source, 0});
//    q.push_back({flow.target, 1});
//    bool f = false;
//    tuple<int, int, int> mid;
//    while (q_start < q.size()) {
//        pair<int, int> vv = q[q_start++];
//        int v = vv.first;
//        int c = vv.second;
//
//        if (d[v].first > limit) break;
//
//        int e_id = pr[v] == MAX_EDGE_ID ? 0 : state.ordered_edges[pr[v]].id;
//        for (int i = state.ordered_edges_ids[v].first; i < state.ordered_edges_ids[v].second; i++) {
//            const auto &e = state.ordered_edges[i];
//            if (state.node_flow_size[e.v] >= node_flow_limit) continue;
//            if (state.group_id_flow_size[e.group_id] >= group_flow_limit) continue;
//            if (state.edge_capacity[e.id] < flow.flow) continue;
//            if (state.blocked[blocked_hacher({e_id, e.id})]) continue;
//            if (d[e.v].first != 1e9 && d[e.v].second != c &&
//                !state.blocked[blocked_hacher({e_id, state.ordered_edges[pr[e.v]].id})]) {
//                mid = {v, i, e.v};
//                f = true;
//                break;
//            }
//            if (d[e.v].first != 1e9) continue;
//            d[e.v] = {d[v].first + 1, c};
//            q.push_back({e.v, c});
//            pr[e.v] = i;
//        }
//        if (f) break;
//    }
//    if (!f) return nullopt;
////    std::cerr << "Found path for " << flow.id << std::endl;
//    vector<int> ans0, ans1;
//    {
//        int v = get<0>(mid);
//        while (true) {
//            if (v == flow.source || v == flow.target) break;
//            ans0.push_back(pr[v]);
//            int e_id = state.ordered_edges[pr[v]].id;
//            int to = in.edges[e_id].v == v ? in.edges[e_id].u : in.edges[e_id].v;
//            v = to;
//        }
//    }
//    {
//        int v = get<2>(mid);
//        while (true) {
//            if (v == flow.source || v == flow.target) break;
//            ans1.push_back(pr[v]);
//            int e_id = state.ordered_edges[pr[v]].id;
//            int to = in.edges[e_id].v == v ? in.edges[e_id].u : in.edges[e_id].v;
//            v = to;
//        }
//        int mid_e_id = get<1>(mid);
//        if (v == flow.target) {
//            // source -> v -> mid_e_id -> e.v -> target
//            for (auto &id: ans1) {
//                int e_id = state.ordered_edges[id].id;
//                const auto &p = state.input_in_ordered_edges[e_id];
//                int id_ = p.first == id ? p.second : p.first;
//                id = id_;
//            }
//            reverse(ans0.begin(), ans0.end());
//            ans0.push_back(mid_e_id);
//            copy(ans1.begin(), ans1.end(), back_inserter(ans0));
//            return ans0;
//        } else {
//            // target -> v -> mid_e_id -> e.v -> source
//            reverse(ans1.begin(), ans1.end());
//            reverse(ans0.begin(), ans0.end());
//            ans0.push_back(mid_e_id);
//            for (auto &id: ans0) {
//                int e_id = state.ordered_edges[id].id;
//                const auto &p = state.input_in_ordered_edges[e_id];
//                int id_ = p.first == id ? p.second : p.first;
//                id = id_;
//            }
//            copy(ans0.rbegin(), ans0.rend(), back_inserter(ans1));
//            return ans1;
//        }
//    }
//}

void add_state(const Flow &flow, const vector<int> &path, int add) {
    state.node_flow_size[flow.source] += add;
    for (int id: path) {
        int e_id = state.ordered_edges[id].id;
        state.edge_capacity[e_id] -= add > 0 ? flow.flow : -flow.flow;
        state.group_id_flow_size[in.edges[e_id].group_id] += add;
        state.node_flow_size[state.ordered_edges[id].v] += add;
    }
}

void solve() {
    Output out;

    init_state();

    sort(in.flows.begin(), in.flows.end(), [](const Flow &a, const Flow &b) {
        return a.flow < b.flow;
    });

    bitset<15000> found;
    for (int j = 6; j < 9; j++) {
        for (int i = 0; i < 6; i++) {
            for (const auto &flow : in.flows) {
                if (found[flow.id]) continue;
                auto path_opt = find_path(flow, 1 << i, min<int>(SFL, 1 << j), min<int>(GFL, 1 << j));
                if (path_opt) {
                    found[flow.id] = 1;
                    add_state(flow, path_opt.value(), 1);
                    out.paths.emplace_back(flow.id, std::move(path_opt.value()));
                }
            }
        }
    }
    while (1.0 * clock() / CLOCKS_PER_SEC < 1.95) {
        shuffle(out.paths.begin(), out.paths.end(), rnd);
        for (auto &it: out.paths) {
            if (1.0 * clock() / CLOCKS_PER_SEC > 1.95) break;

            const auto &flow = state.flow_by_id[it.first];
            add_state(flow, it.second, -1);
            for (auto id: it.second) {
                state.is_node_blocked[state.ordered_edges[id].v] = true;
            }
            state.is_node_blocked[flow.target] = false;
            auto path_opt = find_path(flow, (int)(it.second.size()) * 2, SFL, GFL);
            if (path_opt) {
                add_state(flow, path_opt.value(), 1);
                it.second = std::move(path_opt.value());
            } else {
                add_state(flow, it.second, 1);
            }
            for (auto id: it.second) {
                state.is_node_blocked[state.ordered_edges[id].v] = false;
            }
        }
        for (const auto &flow : in.flows) {
            if (1.0 * clock() / CLOCKS_PER_SEC > 1.95) break;

            if (found[flow.id]) continue;
            auto path_opt = find_path(flow, 50);
            if (path_opt) {
                found[flow.id] = 1;
                add_state(flow, path_opt.value(), 1);
                out.paths.emplace_back(flow.id, std::move(path_opt.value()));
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

    assert(out.paths.size() > 0);
    cout << out;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    srand(time(0));

    cin >> in;
    solve();
    return 0;
}
