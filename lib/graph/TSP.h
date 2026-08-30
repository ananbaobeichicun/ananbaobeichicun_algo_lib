#ifndef TSP_H
#define TSP_H
#include <vector>
#include <cstdint>

static size_t TSP(const std::vector<std::vector<std::pair<size_t, uint64_t> > > &edges) {
    const size_t n = edges.size();
    std::vector dp(1uz << n, std::vector(n, 0x3f3f3f3full));
    dp[1][0] = 0;
    for (size_t i = 1; i < 1 << n; ++i)
        for (size_t u = 0; u < n; ++u)
            for (const auto [v, d]: edges[u])
                if (!(i & 1uz << v))
                    dp[i | 1uz << v][v] = std::min(dp[i | 1uz << v][v], dp[i][u] + d);

    return std::ranges::min(dp[(1uz << n) - 1]);
}
#endif //TSP_H
