#ifndef RMPQ_BIT_H
#define RMPQ_BIT_H
#include "binary_indexed_tree.h"

template<typename T>
    requires requires(T t)
    {
        { t + t } -> std::same_as<T>;
        { t += t } -> std::same_as<T &>;
        { -t } -> std::same_as<T>;
    }
struct RMPQ_BIT {
    binary_indexed_tree<T> tree;

    constexpr RMPQ_BIT() = default;

    explicit constexpr RMPQ_BIT(const size_t n): tree(n) {
    }

    template<std::ranges::input_range R>
        requires requires(T t)
        {
            { t - t } -> std::same_as<T>;
        } && std::is_same_v<std::ranges::range_value_t<R>, T>
    explicit constexpr RMPQ_BIT(const R &v): tree(v, from_sum_tag) {
    }


    constexpr size_t size() const {
        return tree.size();
    }

    constexpr void add(size_t b, size_t e, T delta) {
        tree.add(b, delta);
        if (e + 1 < size())
            tree.add(e + 1, -delta);
    }

    constexpr T query(size_t idx) const {
        return tree.sum(idx);
    }
};

#endif //RMPQ_BIT_H
