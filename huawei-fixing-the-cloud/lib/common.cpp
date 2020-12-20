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

}  // namespace utils