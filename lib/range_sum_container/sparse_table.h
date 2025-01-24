#ifndef SPARSE_TABLE_H
#define SPARSE_TABLE_H
#include <vector>

#include "group_concept.hpp"
#include "segment.h"

template<semigroup T>
    requires idempotent<T>
struct sparse_table {
    std::vector<std::vector<T> > data;
    size_t _size;

    constexpr explicit sparse_table(const size_t n): data(n, std::vector<T>(std::__lg(n) + 1)), _size(n) {
    }

    constexpr explicit
    sparse_table(const std::vector<T> &v): sparse_table(v.size()) {
        for (size_t j = 0; j < std::__lg(_size) + 1; ++j) {
            for (size_t i = 0; i < _size; ++i) {
                if (!j)
                    data[i][j] = v[i];
                else if (i + (1 << j - 1) < _size)
                    data[i][j] = data[i][j - 1] + data[i + (1 << j - 1)][j - 1];
            }
        }
    }

    constexpr size_t size() const {
        return _size;
    }

    constexpr T query(const segment seg) const {
        assert(seg.len() && seg.is_sub_interval_of({0, _size}));
        const size_t l = std::__lg(seg.len());
        return data[seg.b][l] + data[seg.e - (1 << l)][l];
    }

    constexpr T operator[](size_t idx) {
        return data[idx][0];
    }

    constexpr const T &operator[](size_t idx) const {
        return data[idx][0];
    }
};
#endif //SPARSE_TABLE_H
