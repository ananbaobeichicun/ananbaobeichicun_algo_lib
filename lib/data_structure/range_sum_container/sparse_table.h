#ifndef SPARSE_TABLE_H
#define SPARSE_TABLE_H
#include <cassert>
#include <vector>

#include "../../concepts/group_concepts.h"

template<std::unsigned_integral T>
T saturating_sub(const T x, const T y) {
    return x > y ? x - y : 0;
}

template<semigroup T>
    requires idempotent<T>
struct sparse_table {
    std::vector<std::vector<T> > data;
    size_t _size;

    constexpr explicit sparse_table(const size_t n) : data((size_t) std::bit_width(n), std::vector<T>(n)), _size(n) {
    }

    template<std::ranges::input_range R>
        requires std::ranges::sized_range<R>
                 && std::indirectly_copyable<std::ranges::iterator_t<R>, typename std::vector<T>::iterator>
    constexpr explicit sparse_table(const R &r) : sparse_table(r.size()) {
        if (!r.size())
            return;
        std::ranges::copy(r, data[0].begin());
        for (size_t i = 1; i < (size_t) std::bit_width(_size); ++i)
            for (size_t j = 0; j + (1uz << i) - 1 < _size; ++j)
                update(i, j);
    }

    void update(const size_t i, const size_t j) {
        data[i][j] = data[i - 1][j] + data[i - 1][j + (1uz << (i - 1))];
    }

    [[nodiscard]] constexpr size_t size() const {
        return _size;
    }

    [[nodiscard]] constexpr T query(const size_t l, const size_t r) const {
        assert(l < r && r <= _size);
        const size_t lg = (size_t) std::bit_width(r - l) - 1;
        return data[lg][l] + data[lg][r - (1uz << lg)];
    }

    /**
     * 区间赋值操作
     * 复杂度 O(_size + (r - l) * log(_size))
     */
    constexpr void modify(const size_t l, const size_t r, const T val) {
        std::fill(data[0].begin() + l, data[0].begin() + r, val);
        for (size_t i = 1; i < std::bit_width(_size); ++i)
            for (size_t j = saturating_sub(l, 1uz << i); j + (1uz << i) - 1 < _size && j < r; ++j)
                update(i, j);
    }

    constexpr T operator[](size_t idx) {
        return data[0][idx];
    }

    constexpr const T &operator[](size_t idx) const {
        return data[0][idx];
    }
};
#endif //SPARSE_TABLE_H
