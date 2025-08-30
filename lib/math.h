#ifndef MATH_H
#define MATH_H

#include <cassert>
#include <cmath>

namespace norms {
    constexpr double norm_squared(const double x, const double y) {
        return x * x + y * y;
    }

    constexpr double norm(const double x, const double y) {
        return std::sqrt(norm_squared(x, y));
    }

    constexpr double rev_norm_squared(const double x, const double y) {
        assert(x > y);
        return x * x - y * y;
    }

    constexpr double rev_norm(const double x, const double y) {
        return std::sqrt(rev_norm_squared(x, y));
    }
}

namespace fast_pow_n {
    using ull = unsigned long long;

    ull fast_pow(ull a, ull b, const ull p = std::numeric_limits<ull>::max()) {
        ull ans = 1;
        do {
            if (b & 1)
                ans = ans * a % p;
            a = a * a % p;
        } while (b >>= 1);
        return ans;
    }
}

template<std::unsigned_integral T>
T ilog(const T base, const T x) {
    if (!x || base <= 1)
        throw std::exception{};
    if (x < base)
        return 0;
    T ans = 0, acc = base;
    while (acc * acc < x) {
        acc *= acc;
        ++ans;
    }

    return (1 << ans) + ilog(base, x / acc);
}

template<std::integral T>
constexpr T aver(T x, T y) {
    return ((x ^ y) >> 1) + (x & y);
}

template<std::unsigned_integral T>
constexpr T isqrt(const T x) {
    if (x <= 1)
        return x;
    const T res = isqrt(x >> 2) << 1, gres = res | 1;
    return gres * gres <= x ? gres : res;
}

template<std::integral T>
constexpr T difference(T x, T y) {
    return x < y ? y - x : x - y;
}

constexpr double clamped_asin(const double x) {
    return std::asin(std::clamp(x, -1., 1.));
}

constexpr double clamped_acos(const double x) {
    return std::acos(std::clamp(x, -1., 1.));
}
#endif //MATH_H
