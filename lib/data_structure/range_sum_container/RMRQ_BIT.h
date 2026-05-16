#ifndef RMRQ_BIT_H
#define RMRQ_BIT_H
#include "binary_indexed_tree.h"

template<typename T>
    requires requires(T t, size_t times)
    {
        { t + t } -> std::same_as<T>;
        { t += t } -> std::same_as<T &>;
        { t - t } -> std::same_as<T>;
        // { times * t } -> std::same_as<T>;
        // { t *= times } -> std::same_as<T &>;
    }
struct RMRQ_BIT {
    binary_indexed_tree<T> d, kd;

    constexpr RMRQ_BIT() = default;

    explicit constexpr RMRQ_BIT(size_t n): d(n), kd(n) {
    }

    template<std::ranges::input_range R>
        requires std::is_same_v<std::ranges::range_value_t<R>, T>
    explicit constexpr RMRQ_BIT(const R &v): d(v, from_sum_tag) {
        std::vector<T> diff; //todo reserve
        std::adjacent_difference(v.begin(), v.end(), std::back_inserter(diff));
        for (size_t i = 0; i < diff.size(); ++i)
            diff[i] *= i;
        kd = binary_indexed_tree<T>(diff, from_origin_tag);
    }

    constexpr size_t size() const {
        return d.size();
    }

    constexpr void add(const size_t b, const size_t e, const T &delta) {
        d.add(b, delta);
        kd.add(b, b * delta);
        if (e + 1 < size()) {
            d.add(e + 1, -delta);
            kd.add(e + 1, -(e + 1) * delta);
        }
    }

    constexpr T sum(const size_t idx) const {
        return (idx + 1) * d.sum(idx) - kd.sum(idx);
    }

    constexpr T query(const size_t b, const size_t e) const {
        return sum(e) - (b ? sum(b - 1) : 0);
    }
};

#endif //RMRQ_BIT_H
