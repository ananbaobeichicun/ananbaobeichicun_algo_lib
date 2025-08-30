#ifndef SPARSE_TABLE_H
#define SPARSE_TABLE_H
#include <cassert>
#include <vector>

#include "group_concept.h"
#include "segment.h"

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
        for (size_t j = 1; j < std::__lg(_size) + 1; ++j)
            for (size_t i = 0; i < _size; ++i)
                if (i + (1 << j - 1) < _size)
                    data[j][i] = data[j - 1][i] + data[j - 1][i + (1 << j - 1)];
    }

    constexpr size_t size() const {
        return _size;
    }

    constexpr T query(const segment<size_t> seg) const {
        assert(seg.len() && seg <= (segment<size_t>{0, _size}));
        const size_t l = std::__lg(seg.len());
        return data[l][seg.b] + data[l][seg.e - (1 << l)];
    }

    constexpr T operator[](size_t idx) {
        return data[0][idx];
    }

    constexpr const T &operator[](size_t idx) const {
        return data[0][idx];
    }
};
#endif //SPARSE_TABLE_H
