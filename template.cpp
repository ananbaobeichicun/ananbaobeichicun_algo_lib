// #define NDEBUG
#include <bits/stdc++.h>
#include <cassert>

#if defined(LOCAL_DBG) && !defined(ONLINE_JUDGE)
#define dbg(x) std::println(std::cerr, "\x1b[94m{}:\x1b[39;49m {}", #x, x)
#include "lib/third_party/nanobench.h"
#else
#define dbg(x)
#define size_t uint64_t
#endif

using uc = unsigned char;
using ll = int64_t;
using ull = uint64_t;
using ld = long double;

static void solve() {
    size_t n;
    std::cin >> n;
    std::vector<size_t> v(n);

    for (size_t i = 0; i < n; ++i)
        std::cin >> v[i];

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout << std::fixed << std::setprecision(20);
    // int size(256 << 20); // 256M
    // __asm__ ( "movq %0, %%rsp\n"::"r"((char *) malloc(size) + size));

    size_t t = 1;
    // std::cin >> t;
    while (t--)
        solve();

    std::exit(EXIT_SUCCESS);
}
