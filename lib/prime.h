#ifndef PRIME_H
#define PRIME_H

#include <utility>
#include <vector>
#include <map>
#include <bitset>
#include <ranges>
#include "concepts/integral_concepts.h"

using ull = unsigned long long;

template<generalized_unsigned_integral T>
constexpr bool is_prime_naive(const T n) {
    if (n < 2)
        return false;
    for (T i = 2; i * i <= n; ++i)
        if (n % i == 0)
            return false;
    return true;
}

template<generalized_unsigned_integral T, std::output_iterator<T> It>
void get_factors(T n, It it) {
    if (!n)
        throw std::invalid_argument{""};

    for (T i = 2; i * i <= n; ++i) {
        while (n % i == 0) {
            *it++ = i;
            n /= i;
        }
    }

    if (n != 1)
        *it++ = n;
}
#endif //PRIME_H
