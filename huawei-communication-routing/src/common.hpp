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