#ifndef UTILS_H
#define UTILS_H
#include <concepts>

template<std::signed_integral T>
constexpr T euclid_mod(const T a, const T b) {
    return (a % b + b) % b;
}

#endif //UTILS_H