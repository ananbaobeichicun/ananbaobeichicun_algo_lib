#ifndef DISCRETE_H
#define DISCRETE_H
#include <vector>
#include <algorithm>

template<std::ranges::random_access_range R, typename T = typename R::value_type>
[[nodiscard]] constexpr std::vector<size_t> coordinate_compress(const R &v) {
    const size_t n = v.size();
    std::vector<T> tmp = v;
    std::ranges::sort(tmp);
    tmp.erase(std::ranges::unique(tmp).begin(), tmp.end());
    std::vector<size_t> ans(n);
    for (size_t i = 0; i < n; ++i)
        ans[i] = std::ranges::lower_bound(tmp, v[i]) - tmp.begin();
    return ans;
}

#endif //DISCRETE_H
