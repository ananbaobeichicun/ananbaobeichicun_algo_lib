#ifndef DISCRETE_H
#define DISCRETE_H
#include <vector>
#include <algorithm>

template<std::ranges::random_access_range R, std::weakly_incrementable O>
    requires std::indirectly_writable<O, size_t>
constexpr void coordinate_compress(const R &v, O it) {
    using T = typename R::value_type;
    const size_t n = v.size();
    // std::vector<T> tmp = v;
    std::vector<T> tmp(std::from_range, v);
    std::ranges::sort(tmp);
    tmp.erase(std::ranges::unique(tmp).begin(), tmp.end());

    for (size_t i = 0; i < n; ++i)
        *it++ = static_cast<size_t>(std::ranges::lower_bound(tmp, v[i]) - tmp.begin());
}

#endif //DISCRETE_H
