#ifndef MATH_H
#define MATH_H

#include <algorithm>
#include <cmath>

template<std::unsigned_integral T>
T fast_pow(T a, T b, const T p = std::numeric_limits<T>::max()) {
    T ans = 1;
    do {
        if (b & 1)
            ans = ans * a % p;
        a = a * a % p;
    } while (b >>= 1);
    return ans;
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

template<std::unsigned_integral T>
constexpr T isqrt(const T x) {
    if (x <= 1)
        return x;
    const T res = isqrt(x >> 2) << 1, gres = res | 1;
    return gres * gres <= x ? gres : res;
}

template<typename T>
    requires std::integral<T> || std::floating_point<T>
constexpr T difference(T x, T y) {
    return x < y ? y - x : x - y;
}

template<std::unsigned_integral T>
constexpr T saturating_sub(const T x, const T y) {
    return x > y ? x - y : 0;
}


constexpr bool double_eq(const double a, const double b) {
    constexpr double eps = 1e-9;
    return std::abs(a - b) < eps;
}

constexpr double clamped_asin(const double x) {
    return std::asin(std::clamp(x, -1., 1.));
}

constexpr double clamped_acos(const double x) {
    return std::acos(std::clamp(x, -1., 1.));
}
#endif //MATH_H
