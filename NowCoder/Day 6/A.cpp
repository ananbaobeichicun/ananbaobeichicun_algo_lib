#include <iostream>
#include <cassert>
#include <format>
#include <ranges>
#include <vector>
#include <algorithm>

using ull = unsigned long long;
using ll = long long;

struct pair : std::pair<ull, ull> {
    using std::pair<ull, ull>::pair;
};

std::strong_ordering operator<=>(const pair &x, const pair &y) {
    if (x.first + x.second < y.first + y.second)
        return std::strong_ordering::less;
    if (x.first + x.second > y.first + y.second)
        return std::strong_ordering::greater;
    if (x.first > y.first)
        return std::strong_ordering::less;
    if (x.first < y.first)
        return std::strong_ordering::greater;
    return std::strong_ordering::equal;
}

void solve() {
    size_t n;
    std::cin >> n;

    std::vector<pair> v(n);
    for (auto &[a, b]: v)
        std::cin >> a >> b;

    std::ranges::sort(v);

    ull ans = 0, mx = 0;
    for (const auto [a, b]: v) {
        ull nans = std::max(ans, a);
        if (ans >= a)
            nans = a + b;
        else
            nans = std::max(nans, mx);
        ans = nans;
        mx = std::max(mx, a + b);
    }

    std::cout << ans << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    size_t t = 1;
    std::cin >> t;
    while (t--)
        solve();
}
