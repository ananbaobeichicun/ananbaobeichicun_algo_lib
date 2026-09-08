#ifndef MATH_H
#define MATH_H
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>

template<std::unsigned_integral T>
static T fast_pow(T a, T b, const T p = std::numeric_limits<T>::max()) {
    T ans = 1;
    do {
        if (b & 1)
            ans = ans * a % p;
        a = a * a % p;
    } while (b >>= 1);
    return ans;
}

template<std::unsigned_integral T>
static T ilog(const T base, const T x) {
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
constexpr static T isqrt(const T x) {
    if (!x)
        return x;
    const T res = isqrt(x >> 2) << 1, gres = res | 1;
    return gres * gres <= x ? gres : res;
}

template<std::unsigned_integral T>
constexpr static T cisqrt(const T x) {
    T res = isqrt(x);
    return res * res == x ? res : res + 1;
}

template<typename T>
    requires std::integral<T> || std::floating_point<T>
constexpr static T difference(T x, T y) {
    return x < y ? y - x : x - y;
}

template<std::unsigned_integral T>
constexpr static T saturating_sub(const T x, const T y) {
    return x > y ? x - y : 0;
}

template<std::unsigned_integral T>
static T floor_tmxd(std::vector<T> v, const T k) {
    const size_t n = v.size();
    assert(k && k <= n);
    std::ranges::sort(v);
    std::partial_sum(v.begin(), v.end(), v.begin());
    T ans = ~0uz;
    for (size_t i = 0; i < k; ++i)
        ans = std::min(ans, v[n - 1 - i] / (k - i));
    return ans;
}

template<std::unsigned_integral T>
static T ceil_tmxd(std::vector<T> v, const T k) {
    const size_t n = v.size();
    assert(k && k <= n);
    return std::max(std::ranges::max(v), std::reduce(v.begin(), v.end(), T{0}) / k);
}

template<typename T>
concept float_like = std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, long double>;

template<float_like T>
constexpr static bool is_close(const T a, const T b) {
    constexpr T eps = 1e-9;
    return std::abs(a - b) < eps;
}

template<float_like T>
constexpr static T clamped_asin(const T x) {
    return std::asin(std::clamp(x, -1., 1.));
}

template<float_like T>
constexpr static T clamped_acos(const T x) {
    return std::acos(std::clamp(x, -1., 1.));
}
#endif //MATH_H
