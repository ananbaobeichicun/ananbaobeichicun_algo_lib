#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stack>
#include <utility>

using ull = unsigned long long;
using ll = long long;

void solve() {
    size_t n, m, k;
    std::cin >> n >> m >> k;
    std::stack<ull> stk;
    std::vector<ull> buffer;
    buffer.reserve(k);

    std::optional<ull> remanence;

    auto flush = [&] {
        std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<ull>{std::cout, " "});
        std::cout << "\n";
        buffer.resize(0);
    };

    bool flushed = false;
    auto attempt = [&](const ull x) {
        if (!buffer.empty() && x > buffer.back())
            return false;
        if (buffer.size() == k) {
            flush();
            flushed = true;
            return false;
        }

        buffer.push_back(x);
        return true;
    };

    while (!std::cin.eof() || !stk.empty()) {
        if (!stk.empty() && attempt(stk.top())) {
            stk.pop();
            continue;
        }

        if (std::cin.eof()) {
            flush();
        } else {
            ull cur;
            if (remanence) {
                cur = *std::exchange(remanence, std::nullopt);
            } else {
                std::cin >> cur;
            }
            if (!attempt(cur)) {
                if (flushed) {
                    flushed = false;
                    continue;
                }
                if (stk.size() == m) {
                    remanence = cur;
                    flush();
                } else {
                    stk.push(cur);
                }
            }
        }
    }
    std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<ull>{std::cout, " "});
    std::cout << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    size_t t = 1;
    // std::cin >> t;
    while (t--)
        solve();
}
