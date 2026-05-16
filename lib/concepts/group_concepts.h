#ifndef GROUP_CONCEPT_H
#define GROUP_CONCEPT_H
#include <concepts>

template<typename T>
concept semigroup = requires(T x, T y)
{
    { x + y } -> std::same_as<T>;
    { x += y } -> std::same_as<T &>;
    // axiom(T x, T y, T z): (x + y) + z == x + (y + z)
};

template<typename T>
concept monoid = semigroup<T> && requires
{
    { T::identity } -> std::same_as<T>;
    // axiom(T x): x + T::identity == x && T::identity + x == x
};

template<typename T>
concept group = monoid<T> && requires(T x, T y)
{
    { -x } -> std::same_as<T>;
    // axiom(T x): x + (-x) == T::identity && (-x) + x == T::identity
};

template<typename T>
concept communicate = true; // axiom(T x, T y): x + y == y + x

template<typename T>
concept idempotent = true; // axiom(T x): x + x == x
#endif //GROUP_CONCEPT_H
