#ifndef LINEAR_BASE_H
#define LINEAR_BASE_H
#include <numeric>
#include <algorithm>
#include <stdexcept>

template<std::unsigned_integral T>
struct linear_base {
    constexpr static size_t SIZE = std::numeric_limits<T>::digits;
    std::array<T, SIZE> data{};

    T &operator[](const size_t idx) {
        if (idx >= SIZE)
            throw std::range_error{""};
        return data[idx];
    }

    const T &operator[](const size_t idx) const {
        if (idx >= SIZE)
            throw std::range_error{""};
        return data[idx];
    }

    void clear() {
        std::ranges::fill(data, 0);
    }

    bool empty() const {
        return !std::ranges::any_of(data, std::identity{});
    }

    bool contains(T x) const {
        size_t lg = SIZE - 1;
        while (true) {
            while (~lg && !(1ull << lg & x))
                --lg;
            if (!~lg)
                return true;
            if (!data[lg])
                return false;
            x ^= data[lg];
        }
    }

    bool insert(T x) {
        size_t lg = SIZE - 1;
        while (true) {
            while (~lg && !(1ull << lg & x)) --lg;
            if (!~lg) return false;
            if (!data[lg]) {
                data[lg] = x;
                break;
            }
            x ^= data[lg];
        }
        for (size_t i = lg - 1; ~i; --i)
            if (1ull << i & data[lg])
                data[lg] ^= data[i];
        for (size_t i = lg + 1; i < SIZE; ++i)
            if (1ull << lg & data[i])
                data[i] ^= data[lg];
        return true;
    }

    [[nodiscard]] T query_max() const {
        return std::reduce(data.begin(), data.end(), 0ull, std::bit_xor{});
    }

    T query_kth_min(size_t k) const {
        T ans = 0;
        auto it = data.begin();
        for (; k; k >>= 1) {
            it = std::find_if(it, data.end(), std::identity{});
            if (it == data.end())
                throw std::invalid_argument{""};
            if (k & 1)
                ans ^= *it;
            ++it;
        }
        return ans;
    }

    void merge(const linear_base &other) {
        for (size_t i = 0; i < SIZE; ++i)
            insert(other[i]);
    }
};
#endif //LINEAR_BASE_H
