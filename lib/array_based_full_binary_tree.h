#ifndef ARRAY_BASED_FULL_BINARY_TREE
#define ARRAY_BASED_FULL_BINARY_TREE
#include <array>
#include <cassert>

//array based full binary tree
template<typename T, size_t depth>
class ABFBT {
    constexpr static size_t depth_max = depth - 1;
    using data_t = std::array<T, (1 << depth) + 1>;
    data_t data;

public:
    explicit ABFBT(data_t data = data_t()): data(data) {
    }

    T &operator[](size_t index) {
        return data[index];
    }
};

template<typename T, size_t depth>
class ABFBT_iter {
    constexpr static size_t depth_max = depth - 1;
    ABFBT<T, depth> &source;
    size_t index;

public:
    [[nodiscard]] constexpr size_t get_index() const {
        return index;
    }

    [[nodiscard]] constexpr size_t get_depth() const {
        return std::__lg(index);
    }

    explicit ABFBT_iter(ABFBT<T, depth> &source, const size_t index = 1): source(source), index(index) {
    }

    ABFBT_iter(const ABFBT_iter &that): source(that.source), index(that.index) {
    }

    ABFBT_iter &operator=(const ABFBT_iter &that) {
        assert(&source == &that.source);
        index = that.index;
        return *this;
    }

    ABFBT_iter operator<<(int) const {
        assert(get_depth() < depth_max);
        return ABFBT_iter(source, index << 1);
    }

    ABFBT_iter &operator<<=(int) {
        return *this = *this << 0;
    }

    ABFBT_iter operator>>(int) const {
        assert(get_depth() < depth_max);
        return ABFBT_iter(source, index << 1 | 1);
    }

    ABFBT_iter &operator>>=(int) {
        return *this = *this >> 0;
    }

    T &operator*() {
        return source[index];
    }

    T *operator->() {
        return &source[index];
    }
};
#endif //ARRAY_BASED_FULL_BINARY_TREE
