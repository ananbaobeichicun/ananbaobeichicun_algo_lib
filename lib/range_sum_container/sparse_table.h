#ifndef SPARSE_TABLE_H
#define SPARSE_TABLE_H
#include <cassert>
#include <vector>

#include "../concepts/group_concept.h"
#include "segment.h"

template<std::unsigned_integral T>
T saturating_sub(const T x, const T y) {
    return x > y ? x - y : 0;
}

template<semigroup T>
    requires idempotent<T>
struct sparse_table {
    std::vector<std::vector<T> > data;
    size_t _size;

    constexpr explicit sparse_table(const size_t n): data(std::__lg(n) + 1, std::vector<T>(n)), _size(n) {
    }

    template<std::ranges::input_range R>
        requires std::ranges::sized_range<R>
                 && std::indirectly_copyable<std::ranges::iterator_t<R>, typename std::vector<T>::iterator>
    constexpr explicit sparse_table(const R &r): sparse_table(r.size()) {
        std::ranges::copy(r, data[0].begin());
        for (size_t i = 1; i < std::__lg(_size) + 1; ++i)
            for (size_t j = 0; j + (1uz << i) - 1 < _size; ++j)
                update(i, j);
    }

    void update(const size_t i, const size_t j) {
        data[i][j] = data[i - 1][j] + data[i - 1][j + (1uz << (i - 1))];
    }

    [[nodiscard]] constexpr size_t size() const {
        return _size;
    }

    [[nodiscard]] constexpr T query(const segment<size_t> seg) const {
        assert(seg.len() && seg <= (segment<size_t>{0, _size}));
        const size_t l = std::__lg(seg.len());
        return data[l][seg.b] + data[l][seg.e - (1uz << l)];
    }

    /**
     * 区间赋值操作
     * @param seg 赋值的下标范围
     * @param val 赋成的值
     * 复杂度 O(_size + seg.len() * log(_size))
     */
    constexpr void modify(const segment<size_t> seg, const T val) {
        std::fill(data[0].begin() + seg.b, data[0].begin() + seg.e, val);
        for (size_t i = 1; i < std::__lg(_size) + 1; ++i)
            for (size_t j = saturating_sub(seg.b, 1uz << i); j + (1uz << i) - 1 < _size && j < seg.e; ++j)
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
