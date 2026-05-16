#ifndef UTILS_H
#define UTILS_H
#include <concepts>
#include <cmath>

template<std::signed_integral T>
constexpr T euclid_mod(const T a, const T b) {
    return (a % b + std::abs(b)) % b;
}

#endif //UTILS_H