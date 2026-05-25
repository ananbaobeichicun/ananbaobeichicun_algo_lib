#ifndef ALGEBRA_CONCEPT_H
#define ALGEBRA_CONCEPT_H
#include <concepts>

template<typename T>
concept communicate = true; // axiom(T x, T y): x + y == y + x

template<typename T>
concept idempotent = true; // axiom(T x): x + x == x

template<typename T>
concept semigroup = requires(T x, T y)
{
    { x + y } -> std::convertible_to<T>;
    { x += y } -> std::convertible_to<T &>;
    // axiom(T &x, T y): (x += y) == x + y
    // axiom(T x, T y, T z): (x + y) + z == x + (y + z)
};

template<typename T>
concept monoid = semigroup<T> && requires
{
    { T::identity() } -> std::convertible_to<T>;
    // axiom(T x): x + T::identity == x && T::identity + x == x
};

template<typename T>
concept group = monoid<T> && requires(T x, T y)
{
    { -x } -> std::convertible_to<T>;
    { x - y } -> std::convertible_to<T>;
    { x -= y } -> std::convertible_to<T &>;
    // axiom(T &x, T y) (x -= y) == x - y
    // axiom(T x): x + (-x) == T::identity && (-x) + x == T::identity
};

template<typename T>
concept semiring = semigroup<T> && communicate<T> && requires(T x, T y)
{
    { x * y } -> std::convertible_to<T>;
    { x *= y } -> std::convertible_to<T &>;
    { T::zero() } -> std::convertible_to<T>;
    { T::identity() } -> std::convertible_to<T>;
    // axiom(T &x, T y): (x *= y) == x * y
    // axiom(T x): x + T::zero == x && T::zero + x == x
    // axiom(T x, T y) (x * y) * z = x * (y * z)
    // axiom(T x): x * T::identity == x && T::identity * x == x
    // axiom(T x, T y, T z) (x + y) * z == x * z + y * z && z * (x + y) == z * x + z * y
};

template<typename T>
concept ring = semiring<T> && requires(T x, T y)
{
    { -x } -> std::convertible_to<T>;
    { x - y } -> std::convertible_to<T>;
    { x -= y } -> std::convertible_to<T &>;
    // axiom(T &x, T y) (x -= y) == x - y
    // axiom(T x): x + (-x) == T::zero && (-x) + x == T::zero
};
#endif //ALGEBRA_CONCEPT_H
