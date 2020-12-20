#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;
using Point = pair<int, int>;
using VMMove = tuple<int, int, int>;
using Step = vector<VMMove>;

struct Server {
    int c; // cpu
    int m; // memory
    int id;
    int free_c;
    int free_m;
};

struct VM {
    int c; // cpu
    int m; // memory
    int id;
    int from; // old destination
    int to; // new destination
};

namespace utils {};  // namespace utils

struct Input {
    int n, m;
    vector<Server> servers;
    vector<VM> vms;


    friend ostream& operator<<(ostream& stream, const Input& in) {
        stream << in.n << ' ' << in.m << endl;
        for (int i = 0; i < in.n; i++) {
            stream << in.servers[i].c << ' ' << in.servers[i].m << endl;
        }
        for (int i = 0; i < in.m; i++) {
            stream << in.vms[i].c << ' ' << in.vms[i].m << endl;
        }
        for (int i = 0; i < in.m; i++) {
            stream << in.vms[i].from << ' ' << in.vms[i].to << endl;
        }
        return stream;
    }

    friend istream& operator>>(istream& stream, Input& in) {
        stream >> in.n >> in.m;
        in.servers.resize(in.n);
        for (int i = 0; i < in.n; i++) {
            stream >> in.servers[i].c >> in.servers[i].m;
            in.servers[i].id = i;
            in.servers[i].free_c = in.servers[i].c;
            in.servers[i].free_m = in.servers[i].m;
        }
        in.vms.resize(in.m);
        for (int i = 0; i < in.m; i++) {
            stream >> in.vms[i].c >> in.vms[i].m;
            in.vms[i].id = i;
        }
        for (int i = 0; i < in.m; i++) {
            stream >> in.vms[i].from >> in.vms[i].to;
            in.servers[in.vms[i].from].free_c -= in.vms[i].c;
            in.servers[in.vms[i].from].free_m -= in.vms[i].m;
        }

        return stream;
    }
};

struct Output {
    vector<Step> steps;

    Output() = default;

    void init_test() {
        steps.clear();
    }

    friend ostream& operator<<(ostream& stream, const Output& out) {
        stream << out.steps.size() << endl;
        for (int i = 0; i < out.steps.size(); i++) {
            stream << out.steps[i].size() << endl;
            for (auto &move: out.steps[i]) {
                stream << get<0>(move) << ' ' << get<1>(move) << ' ' << get<2>(move) << endl;
            }
        }
        return stream;
    }
    friend istream& operator>>(istream& stream, Output& out) {
        out.steps.clear();
        int cnt;
        stream >> cnt;
        out.steps.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            int k;
            stream >> k;
            out.steps[i].resize(k);
            for (auto &move: out.steps[i]) {
                stream >> get<0>(move) >> get<1>(move) >> get<2>(move);
            }
        }
        return stream;
    }
};


namespace utils {
bool move_possible(const Input& in, VMMove move) {
    int from, to, id;
    tie(from, to, id) = move;
    if (from == to) return false;
    if (in.servers[to].free_c < in.vms[id].c) return false;
    if (in.servers[to].free_m < in.vms[id].m) return false;
    return true;
}


bool make_move(Input& in, VMMove move, bool check = true, bool change_from = true, bool change_to = true, bool change_vm = true) {
    if (check && !move_possible(in, move)) return false;
    int from, to, id;
    tie(from, to, id) = move;
    if (from == to) return false;
    
    if (change_to) {
        in.servers[to].free_c -= in.vms[id].c;
        in.servers[to].free_m -= in.vms[id].m;
    }
    if (change_from) {
        in.servers[from].free_c += in.vms[id].c;
        in.servers[from].free_m += in.vms[id].m;
    }
    if (change_vm) {
        in.vms[id].from = to;
    }
    return true;
}
bool reset_move(Input& in, VMMove move, bool check = true, bool change_from = true, bool change_to = true, bool change_vm = true) {
    if (check && !move_possible(in, move)) return false;
    int from, to, id;
    tie(from, to, id) = move;
    if (from == to) return true;
    
    if (change_to) {
        in.servers[to].free_c += in.vms[id].c;
        in.servers[to].free_m += in.vms[id].m;
    }
    if (change_from) {
        in.servers[from].free_c -= in.vms[id].c;
        in.servers[from].free_m -= in.vms[id].m;
    }
    // if (change_vm) {
    //     in.vms[id].from = to;
    // }
    return true;
}

void fails(string comment) {
    cerr << comment << endl;
    exit(1);
}

bool check_valid(Input in, const Output& out) {
    
    return true;
}

double score(const Input& in, const Output& out) {
    if (out.steps.size() == 0) return 1e18;
    if (!check_valid(in, out)) {
        return 10000;
    }
    long double memory_move_in_gb = 0;
    for (const auto &step : out.steps) {
        for (const auto &move: step) {
            memory_move_in_gb += in.vms[get<2>(move)].m;
        }
    }
    return (long double)(1000) * log10(memory_move_in_gb * out.steps.size() + 1);
}

}  // namespace utils#include <bits/stdc++.h>

using namespace std;
 
namespace matching{
int n, k;
vector<vector<pair<int, int>>> g;
vector<pair<int, int>> mt, mt1;
vector<int> used;
int used_timer = 0;
 
void init() {
    g.clear();
    g.resize(n);
    mt.assign(n, {-1, -1});
    mt1.assign(n, {-1, -1});
    used.assign(n, 0);
}
 
void clear() {
    used_timer++;
    for (int i = 0; i < n; i++) g[i].clear(); 
    mt.assign(n, {-1, -1});
    mt1.assign(n, {-1, -1});
}
 
bool try_kuhn (int v) {
    if (used[v]==used_timer) return false;
    used[v] = used_timer;
    for (int i = 0; i < g[v].size(); ++i) {
        int to = g[v][i].first;
        if (mt[to].first == -1 || try_kuhn(mt[to].first)) {
            mt[to] = {v, g[v][i].second};
            mt[v] = {to, g[v][i].second};
            return true;
        }
    }
    return false;
}
vector<tuple<int, int, int>> calculate(const Input& in) {
    for (int i = 0; i < n; i++) {
        sort(g[i].begin(), g[i].end(), [&in](pair<int, int> a, pair<int, int> b) {
            int pa = a.second;
            int pb = b.second;
            return in.vms[pa].c + in.vms[pa].m > in.vms[pb].c + in.vms[pb].m;
        });
    }
    for (int run = 1; run; ) {
        run = 0;
        used_timer++;
        for (int i = 0; i < n / 2; i++)
            if (mt[i].first == -1 && try_kuhn(i))
                run = 1;
    }
    vector<tuple<int, int, int>> ans;
    for (int i = 0; i < n / 2; ++i) {
        if (mt[i].first != -1) {
            ans.push_back({i, mt[i].first - n / 2, mt[i].second});
        }
    }
    return ans;
}
}
 
Output solve_matching(Input in) {
    matching::n = in.n * 2;
    matching::init();
 
    Output out;
 
    vector<int> bad_index;
    for (int i = 0; i < in.m; i++) {
        if (in.vms[i].from != in.vms[i].to) {
            bad_index.push_back(i);
        }
    }
    double time = 1.0 * clock() / CLOCKS_PER_SEC;
    bool good = false;
    while (1.0 * clock() / CLOCKS_PER_SEC < time + 0.1) {
        bool f = true;
        bool f_shuffle = false;
        matching::clear();
        for (int i: bad_index) {
            if (in.vms[i].from != in.vms[i].to) {
                VMMove cur_move{in.vms[i].from, in.vms[i].to, in.vms[i].id};
                if (utils::move_possible(in, cur_move)) {
                    f = false;
                    matching::g[in.vms[i].from].push_back({in.vms[i].to + in.n, in.vms[i].id});
                    matching::g[in.vms[i].to + in.n].push_back({in.vms[i].from, in.vms[i].id});
                }
                f_shuffle = true;
            }
        }
        if (!f_shuffle && f) {
            good = true;
            break;
        }
        if (f_shuffle && f) {
            // random_shuffle(bad_index.begin(), bad_index.end());
            for (int i: bad_index) {
                // if (rand() % (int)(bad_index.size() * 0.75 + 2) == 0) break;
                if (in.vms[i].from != in.vms[i].to) {
                    for (int j = 0; j < in.n; j++) {
                        VMMove cur_move{in.vms[i].from, j, in.vms[i].id};
                        if (utils::move_possible(in, cur_move)) {
                            f = false;
                            matching::g[in.vms[i].from].push_back({j + in.n, in.vms[i].id});
                            matching::g[j + in.n].push_back({in.vms[i].from, in.vms[i].id});
                        }
                        f_shuffle = true;
                    }
                }
            }
        }
        vector<tuple<int, int, int>> moves = matching::calculate(in);
        vector<tuple<int, int, int>> good_moves;
        for (auto cur_move: moves) {
            if (utils::make_move(in, cur_move, true, false, true, false)) {
                good_moves.push_back(cur_move);
            }
        }
        out.steps.push_back({});
        for (auto cur_move: good_moves) {
            if (utils::make_move(in, cur_move, false, true, false, true)) {
                out.steps.back().push_back(cur_move);
            }
        }
    }
    
    if (!good) out.steps.clear();
    return out;
}
 
mt19937 rnd(228);
 
Input reverse_input(Input in) {
    for (int i = 0; i < in.n; i++) {
        in.servers[i].free_c = in.servers[i].c;
        in.servers[i].free_m = in.servers[i].m;
    }
    for (int i = 0; i < in.m; i++) {
        swap(in.vms[i].from, in.vms[i].to);
        in.servers[in.vms[i].from].free_c -= in.vms[i].c;
        in.servers[in.vms[i].from].free_m -= in.vms[i].m;
    }
    return in;
}
 
Output reverse_output(Output out) {
    reverse(out.steps.begin(), out.steps.end());
    for (auto &step: out.steps) {
        for (auto &move: step) {
            swap(get<0>(move), get<1>(move));
        }
    }
    return out;
}
 
Output solve_random(Input in, double TIME = 2.8) {
    Input in1 = in;
    Output out;
    long double best_score = 1e18;
 
    double time = 1.0 * clock() / CLOCKS_PER_SEC;
    vector<int> IDS;
    for (int i = 0; i < in.m; i++) {
        if (in.vms[i].from != in.vms[i].to) IDS.push_back(i);
    }
    shuffle(IDS.begin(), IDS.end(), rnd);
 
    Output cur_out;
    vector<int> ids, ids_copy;
    vector<int> cnt(in.n, 0);
    vector<int> new_ids;
 
    int tick = ids.size() / 3;
 
    while (1.0 * clock() / CLOCKS_PER_SEC < time + TIME) {
        tick = (tick - 1);
        if (tick < 0) {
            tick = ids.size() / 2;
            shuffle(IDS.begin(), IDS.end(), rnd);
        }
        
        in = in1;
        ids = IDS;
        ids_copy = ids;
        cur_out.steps.clear();
        
        int p = 0;
 
        while (p < ids.size()) {
            // shuffle(ids.begin() + p, ids.end(), rnd);
            int l = p + rnd() % (ids.size() - p);
            int r = p + rnd() % (ids.size() - p);
            if (l > r) swap(l, r);
            shuffle(ids.begin() + l, ids.begin() + r, rnd);
 
            // for (int i = p; i < ids.size(); i++) {
            //     int j = i + rnd() % max(1, (int)((ids.size() - i) * min(0.99, (1.0 * p / ids.size() + 0.05 * tick))));
            //     swap(ids[i], ids[j]);
            // }
            new_ids.clear();
            cur_out.steps.push_back({});
            for (int i = p; i < ids.size(); i++) {
                int id = ids[i];
                VMMove cur_move{in.vms[id].from, in.vms[id].to, in.vms[id].id};
 
                if (cnt[in.vms[id].from] < 2 && cnt[in.vms[id].to] < 2 && utils::make_move(in, cur_move, true, false, true, false)) {
                    ++cnt[in.vms[id].from];
                    ++cnt[in.vms[id].to];
                    ids[p++] = id;
                } else {
                    new_ids.emplace_back(id);
                    continue;
                }
                cur_out.steps.back().emplace_back(std::move(cur_move));
            }
            for (auto move : cur_out.steps.back()) {
                utils::make_move(in, move, false, true, false, true);
                --cnt[get<0>(move)];
                --cnt[get<1>(move)];
            }
            if (cur_out.steps.back().size() == 0) {
                cur_out.steps.clear();
                break;
            }
            copy(new_ids.begin(), new_ids.end(), ids.begin() + p); //
        }
        long double cur_score = utils::score(in1, cur_out);
        if (cur_out.steps.size() > 0 && cur_score < best_score) {
            out = cur_out;
            best_score = cur_score;
            IDS = ids;
        }
    }
    return out;
}
Output solve_random1(Input in, double TIME = 2.8) {
    Input in1 = in;
    Output out;
    long double best_score = 1e18;
 
    double time = 1.0 * clock() / CLOCKS_PER_SEC;
    vector<int> IDS;
    for (int i = 0; i < in.m; i++) {
        if (in.vms[i].from != in.vms[i].to) IDS.push_back(i);
    }
    shuffle(IDS.begin(), IDS.end(), rnd);
 
    Output cur_out;
    vector<int> ids, ids_copy;
    vector<int> cnt(in.n, 0);
    vector<int> new_ids;
 
    int tick = ids.size() / 4;
    int locked = 1;
 
    while (1.0 * clock() / CLOCKS_PER_SEC < time + TIME) {
        tick = (tick - 1);
        locked += 3;
        if (tick < 0) {
            tick = ids.size() / 4;
            locked = 1;
            shuffle(IDS.begin(), IDS.end(), rnd);
        }
        
        in = in1;
        ids = IDS;
        ids_copy = ids;
        cur_out.steps.clear();
        
        int p = 0;
 
        while (p < ids.size()) {
            // shuffle(ids.begin() + p, ids.end(), rnd);
            int l = max(p, locked - 1) + rnd() % (ids.size() - max(p, locked - 1));
            int r = max(p, locked - 1) + rnd() % (ids.size() - max(p, locked - 1));
            if (l > r) swap(l, r);
            shuffle(ids.begin() + l, ids.begin() + r, rnd);
 
            // for (int i = p; i < ids.size(); i++) {
            //     int j = i + rnd() % max(1, (int)((ids.size() - i) * min(0.99, (1.0 * p / ids.size() + 0.05 * tick))));
            //     swap(ids[i], ids[j]);
            // }
            new_ids.clear();
            cur_out.steps.push_back({});
            for (int i = p; i < ids.size(); i++) {
                int id = ids[i];
                VMMove cur_move{in.vms[id].from, in.vms[id].to, in.vms[id].id};
 
                if (cnt[in.vms[id].from] < 2 && cnt[in.vms[id].to] < 2 && utils::make_move(in, cur_move, true, false, true, false)) {
                    ++cnt[in.vms[id].from];
                    ++cnt[in.vms[id].to];
                    ids[p++] = id;
                } else {
                    new_ids.emplace_back(id);
                    continue;
                }
                cur_out.steps.back().emplace_back(std::move(cur_move));
            }
            for (auto move : cur_out.steps.back()) {
                utils::make_move(in, move, false, true, false, true);
                --cnt[get<0>(move)];
                --cnt[get<1>(move)];
            }
            if (cur_out.steps.back().size() == 0) {
                cur_out.steps.clear();
                break;
            }
            copy(new_ids.begin(), new_ids.end(), ids.begin() + p); //
        }
        long double cur_score = utils::score(in1, cur_out);
        if (cur_out.steps.size() > 0 && cur_score < best_score) {
            out = cur_out;
            best_score = cur_score;
            IDS = ids;
        }
    }
    return out;
}
Output solve_random2(Input in, Output out1, double TIME = 2.8) {
    Input in1 = in;
    Output out;
    long double best_score = 1e18;
 
    double time = 1.0 * clock() / CLOCKS_PER_SEC;
    vector<int> IDS;
    for (auto step: out1.steps) for (auto move: step) IDS.push_back(get<2>(move));
    // shuffle(IDS.begin(), IDS.end(), rnd);
 
    Output cur_out;
    vector<int> ids, ids_copy;
    vector<int> cnt(in.n, 0);
    vector<int> new_ids;
 
    int tick = ids.size() / 3;
 
    while (1.0 * clock() / CLOCKS_PER_SEC < time + TIME) {
        // tick = (tick - 1);
        // if (tick < 0) {
        //     tick = ids.size() / 2;
        //     shuffle(IDS.begin(), IDS.end(), rnd);
        // }
        
        in = in1;
        ids = IDS;
        ids_copy = ids;
        cur_out.steps.clear();
        
        int p = 0;
 
        while (p < ids.size()) {
            // shuffle(ids.begin() + p, ids.end(), rnd);
            
            int l = p + rnd() % (ids.size() - p);
            int r = p + rnd() % (ids.size() - p);
            if (l > r) swap(l, r);
            swap(ids[l], ids[r]);
 
            // for (int i = p; i < ids.size(); i++) {
            //     int j = i + rnd() % max(1, (int)((ids.size() - i) * min(0.99, (1.0 * p / ids.size() + 0.05 * tick))));
            //     swap(ids[i], ids[j]);
            // }
            new_ids.clear();
            cur_out.steps.push_back({});
            for (int i = p; i < ids.size(); i++) {
                int id = ids[i];
                VMMove cur_move{in.vms[id].from, in.vms[id].to, in.vms[id].id};
 
                if (cnt[in.vms[id].from] < 2 && cnt[in.vms[id].to] < 2 && utils::make_move(in, cur_move, true, false, true, false)) {
                    ++cnt[in.vms[id].from];
                    ++cnt[in.vms[id].to];
                    ids[p++] = id;
                } else {
                    new_ids.emplace_back(id);
                    continue;
                }
                cur_out.steps.back().emplace_back(std::move(cur_move));
            }
            for (auto move : cur_out.steps.back()) {
                utils::make_move(in, move, false, true, false, true);
                --cnt[get<0>(move)];
                --cnt[get<1>(move)];
            }
            if (cur_out.steps.back().size() == 0) {
                cur_out.steps.clear();
                break;
            }
            copy(new_ids.begin(), new_ids.end(), ids.begin() + p); //
        }
        long double cur_score = utils::score(in1, cur_out);
        if (cur_out.steps.size() > 0 && cur_score < best_score) {
            out = cur_out;
            best_score = cur_score;
            IDS = ids;
        }
    }
    return out;
}
Output solve_random3(Input in, Output out1, double TIME = 2.8) {
    Input in1 = in;
    Output out;
    long double best_score = 1e18;
 
    double time = 1.0 * clock() / CLOCKS_PER_SEC;
    vector<int> IDS;
    for (auto step: out1.steps) for (auto move: step) IDS.push_back(get<2>(move));
    // shuffle(IDS.begin(), IDS.end(), rnd);
 
    Output cur_out;
    vector<int> ids, ids_copy;
    vector<int> cnt(in.n, 0);
    vector<int> new_ids;
 
    int tick = ids.size() / 3;
 
    while (1.0 * clock() / CLOCKS_PER_SEC < time + TIME) {
        // tick = (tick - 1);
        // if (tick < 0) {
        //     tick = ids.size() / 2;
        //     shuffle(IDS.begin(), IDS.end(), rnd);
        // }
        
        in = in1;
        ids = IDS;
        ids_copy = ids;
        cur_out.steps.clear();
        
        int p = 0;
 
        while (p < ids.size()) {
            // shuffle(ids.begin() + p, ids.end(), rnd);
            int kek = rnd() % 4 + 1;
            while (kek--) {
                int p1 = p + rnd() % (ids.size() - p);
                int idp1 = ids[p1];
                for (int i = p1 - 1; i >= p; i--) ids[i + 1] = ids[i];
                ids[p] = idp1;
            }
 
            // for (int i = p; i < ids.size(); i++) {
            //     int j = i + rnd() % max(1, (int)((ids.size() - i) * min(0.99, (1.0 * p / ids.size() + 0.05 * tick))));
            //     swap(ids[i], ids[j]);
            // }
            new_ids.clear();
            cur_out.steps.push_back({});
            for (int i = p; i < ids.size(); i++) {
                int id = ids[i];
                VMMove cur_move{in.vms[id].from, in.vms[id].to, in.vms[id].id};
 
                if (cnt[in.vms[id].from] < 2 && cnt[in.vms[id].to] < 2 && utils::make_move(in, cur_move, true, false, true, false)) {
                    ++cnt[in.vms[id].from];
                    ++cnt[in.vms[id].to];
                    ids[p++] = id;
                } else {
                    new_ids.emplace_back(id);
                    continue;
                }
                cur_out.steps.back().emplace_back(std::move(cur_move));
            }
            for (auto move : cur_out.steps.back()) {
                utils::make_move(in, move, false, true, false, true);
                --cnt[get<0>(move)];
                --cnt[get<1>(move)];
            }
            if (cur_out.steps.back().size() == 0) {
                cur_out.steps.clear();
                break;
            }
            copy(new_ids.begin(), new_ids.end(), ids.begin() + p); //
        }
        long double cur_score = utils::score(in1, cur_out);
        if (cur_out.steps.size() > 0 && cur_score < best_score) {
            out = cur_out;
            best_score = cur_score;
            IDS = ids;
        }
    }
    return out;
}
 
void solve() {
    Input in;
    cin >> in;
    Output out = solve_matching(in);
 
    Input in1 = reverse_input(in);
    
    // Output out1 = reverse_output(solve_matching(in1));
    // if (utils::score(in, out) > utils::score(in, out1)) out = out1;
 
    Output out2 = reverse_output(solve_random(in1, 2.2));
    if (utils::score(in, out) > utils::score(in, out2)) out = out2;
    
    rnd = mt19937(1337);

    Output out1 = solve_random1(in, 0.9);
    if (utils::score(in, out) > utils::score(in, out1)) out = out1;

    rnd = mt19937(228);

    Output out3 = solve_random1(in, 0.9);
    if (utils::score(in, out) > utils::score(in, out3)) out = out3;
 
    Output out4 = solve_random2(in, out, 0.6);
    if (utils::score(in, out) > utils::score(in, out4)) out = out4;

    rnd = mt19937(1337);

    Output out41 = solve_random2(in, out, 0.5);
    if (utils::score(in, out) > utils::score(in, out41)) out = out41;
 
    Output out5 = solve_random3(in, out, 0.4);
    if (utils::score(in, out) > utils::score(in, out5)) out = out5;
 
    
    cout << out;
}
 
int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    solve();
    return 0;
}
