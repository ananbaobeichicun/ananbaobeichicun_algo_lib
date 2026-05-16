#include <iostream>
#include <vector>

#ifdef LOCAL_DBG
#include <print>
#define dbg(x) do { println(std::cerr, "\x1b[94m{}:\x1b[39;49m {}", #x, x); } while(false)
#else
#define dbg(x)
#endif

using uc = unsigned char;
using ll = long long;
using ull = unsigned long long;

void solve() {
    size_t n;
    std::cin >> n;
    std::vector<size_t> v(n);

    for (size_t i = 0; i < n; ++i)
        std::cin >> v[i];

}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    // int size(256 << 20); // 256M
    // __asm__ ( "movq %0, %%rsp\n"::"r"((char *) malloc(size) + size));

    size_t t = 1;
    // std::cin >> t;
    while (t--)
        solve();

    exit(EXIT_SUCCESS);
}
