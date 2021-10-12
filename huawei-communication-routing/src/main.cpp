#include "common.hpp" // INSERT_COMMON_HERE

using namespace std;

//const bool DEBUG = false;
const int MAX_EDGE_ID = 15001;
const int MAX_GROUP_ID = 4501;

struct PairHasher {
    size_t operator()(const pair<int, int> &x) const {
        return x.first * MAX_EDGE_ID + x.second;
    }
} blocked_hacher;

struct State {
    vector<Edge> ordered_edges;
    vector<pair<int, int>> ordered_edges_ids;
    vector<int> edge_capacity;
    vector<int> node_flow_size;
    vector<int> group_id_flow_size;
    bitset<MAX_EDGE_ID*MAX_EDGE_ID> blocked;
//    unordered_set<pair<int, int>, PairHasher> constrains;
} state;

void init_state(const Input &in) {
    state.ordered_edges.resize(in.m * 2);
    copy(in.edges.begin(), in.edges.end(), state.ordered_edges.begin());
    copy(in.edges.begin(), in.edges.end(), state.ordered_edges.begin() + in.m);
    for (int i = 0; i < in.m; i++) {
        swap(state.ordered_edges[i].u, state.ordered_edges[i].v);
    }
    sort(state.ordered_edges.begin(), state.ordered_edges.end(), [](const Edge &a, const Edge &b) {
        if (a.u == b.u) return a.capacity < b.capacity;
        return a.u < b.u;
    });

    state.ordered_edges_ids.resize(in.n);
    for (int i = 0; i < in.m * 2; i++) {
        auto cur_u = state.ordered_edges[i].u;
        if (i == 0 || cur_u != state.ordered_edges[i - 1].u) {
            state.ordered_edges_ids[cur_u].first = i;
        }
        state.ordered_edges_ids[cur_u].second = i + 1;
    }

    state.edge_capacity.resize(in.m);
    for (int i = 0; i < in.m; i++) {
        state.edge_capacity[i] = in.edges[i].capacity;
    }

    state.node_flow_size.resize(in.n);

    state.group_id_flow_size.resize(MAX_GROUP_ID);

//    state.constrains.max_load_factor(0.25);
//    state.constrains.reserve(in.constrains.size() * 3);
    for (const auto &constrain: in.constrains) {
        state.blocked[blocked_hacher(constrain.edge_ids)] = 1;
        state.blocked[blocked_hacher({constrain.edge_ids.second, constrain.edge_ids.first})] = 1;
    }
}

optional<vector<int>> find_path(const Flow &flow, const Input &in) {
    if (state.node_flow_size[flow.source] >= SFL) {
        return nullopt;
    }
    vector<int> d(in.n, 1e9);
    vector<int> pr(in.n, MAX_EDGE_ID);
    d[flow.source] = 0;
    queue<int> q;
    q.push(flow.source);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (v == flow.target) break;
        for (int i = state.ordered_edges_ids[v].first; i < state.ordered_edges_ids[v].second; i++) {
            const auto &e = state.ordered_edges[i];
            if (state.node_flow_size[e.v] >= SFL) continue;
            if (state.group_id_flow_size[e.group_id] >= GFL) continue;
            if (state.edge_capacity[e.id] < flow.flow) continue;
            if (state.blocked[blocked_hacher({pr[v], e.id})]) continue;
            if (d[e.v] > d[v] + 1) {
                d[e.v] = d[v] + 1;
                q.push(e.v);
                pr[e.v] = e.id;
            }
        }
    }
    if (d[flow.target] > in.n) return nullopt;
    std::cerr << "Found path for " << flow.id << std::endl;
    vector<int> ans;
    int v = flow.target;
    while (true) {
        int to = in.edges[pr[v]].v == v ? in.edges[pr[v]].u : in.edges[pr[v]].v;
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
    for (int id: path) state.edge_capacity[id] -= flow.flow;
    for (int id: path) ++state.group_id_flow_size[in.edges[id].group_id];
    for (size_t i = 1; i < path.size(); i++) {
        int x = path[i];
        int y = path[i - 1];
        if (in.edges[x].u == in.edges[y].u || in.edges[x].u == in.edges[y].v) {
            ++state.node_flow_size[in.edges[x].u];
        } else {
            ++state.node_flow_size[in.edges[x].v];
        }
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
    for (const auto &flow : in.flows) {
        auto path_opt = find_path(flow, in);
        if (path_opt) {
            if (path_opt.value().size() > 5) continue;
            found.insert(flow.id);
            update_state(flow, path_opt.value(), in);
            out.paths.emplace_back(flow.id, std::move(path_opt.value()));
        }
    }
    for (const auto &flow : in.flows) {
        if (found.count(flow.id)) continue;
        auto path_opt = find_path(flow, in);
        if (path_opt) {
            found.insert(flow.id);
            update_state(flow, path_opt.value(), in);
            out.paths.emplace_back(flow.id, std::move(path_opt.value()));
        }
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