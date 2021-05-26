#include <bits/stdc++.h>
using namespace std;

using Int = long long;
using Double = double;

namespace utils {};  // namespace utils

struct Input {
    friend ostream& operator<<(ostream& stream, const Input& in) {
        return stream;
    }

    friend istream& operator>>(istream& stream, Input& in) {
        return stream;
    }
};

struct Output {
    Output() = default;

    void init_test() {
    }

    friend ostream& operator<<(ostream& stream, const Output& out) {
        return stream;
    }
    friend istream& operator>>(istream& stream, Output& out) {
        return stream;
    }
};

namespace utils {

double score(const Input& in, const Output& out) {
    return 0;
}

}  // namespace utils