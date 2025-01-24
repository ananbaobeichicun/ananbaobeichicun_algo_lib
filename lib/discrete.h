#ifndef DISCRETE_H
#define DISCRETE_H
#include <vector>
#include <algorithm>

template<std::totally_ordered T>
constexpr std::vector<size_t> discrete(const std::vector<T>& v) {
    const size_t n = v.size();
    std::vector<std::pair<T, size_t>> m;
    for (size_t i = 0; i < n; ++i)
        m.emplace_back(v[i], i);
    std::ranges::sort(m);

    std::vector<size_t> ans(n);
    size_t cnt = 0;
    for (size_t i = 0; i < n; ++i) {
        ans[m[i].second] = cnt;
        if (i != n - 1 && m[i].first != m[i + 1].first)
            ++cnt;
    }

    return ans;
}

#endif //DISCRETE_H
