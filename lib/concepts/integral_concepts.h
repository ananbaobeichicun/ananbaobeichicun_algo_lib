#ifndef INTEGRAL_CONCEPTS_H
#define INTEGRAL_CONCEPTS_H
#include <concepts>
using ulll = __uint128_t;
using lll = __int128_t;
template<typename T>
concept generalized_unsigned_integral = std::unsigned_integral<T> || std::is_same_v<T, ulll>;
template<typename T>
concept generalized_signed_integral = std::signed_integral<T> || std::is_same_v<T, lll>;
template<typename T>
concept generalized_integral = generalized_unsigned_integral<T> || generalized_signed_integral<T>;
#endif //INTEGRAL_CONCEPTS_H
