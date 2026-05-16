#ifndef COORDINATE_COMPRESS_H
#define COORDINATE_COMPRESS_H
#include <vector>
#include <algorithm>

template<std::ranges::forward_range R, std::weakly_incrementable O>
    requires std::indirectly_writable<O, size_t>
constexpr O coordinate_compress(const R &v, O it) {
    using T = std::ranges::range_value_t<R>;
    std::vector<T> tmp(std::from_range, v);
    std::ranges::sort(tmp);
    tmp.erase(std::ranges::unique(tmp).begin(), tmp.end());

    return std::ranges::transform(v, it, [&tmp] (const T &val) {
        return static_cast<size_t>(std::ranges::lower_bound(tmp, val) - tmp.begin());
    }).out;
}

#endif //COORDINATE_COMPRESS_H
