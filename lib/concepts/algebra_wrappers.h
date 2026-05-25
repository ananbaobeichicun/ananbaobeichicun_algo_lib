#ifndef UNTITLED_ALGEBRA_WRAPPERS_H
#define UNTITLED_ALGEBRA_WRAPPERS_H
#include <utility>
#include "algebra_concepts.h"

// NOLINTBEGIN(bugprone-reserved-identifier)

template<typename T, typename _Plus>
    requires requires(T x, T y, _Plus plus)
    {
        { plus(x, y) } -> std::convertible_to<T>;
    }
struct semigroup_wrapper {
    using value_type = T;
    using plus = _Plus;

    T data;

    semigroup_wrapper() {
        static_assert(semigroup<semigroup_wrapper>);
    }

    semigroup_wrapper(T data) : data(std::move(data)) {
        static_assert(semigroup<semigroup_wrapper>);
    }

    friend semigroup_wrapper operator+(const semigroup_wrapper &x, const semigroup_wrapper &y) {
        return {_Plus{}(x.data, y.data)};
    }

    friend semigroup_wrapper &operator+=(semigroup_wrapper &x, const semigroup_wrapper &y) {
        return x = x + y;
    }
};

template<typename T, typename _Plus, T _Zero, typename _Multiplies, T _Identity>
    requires requires(T x, T y, _Plus plus, _Multiplies multiplies)
    {
        { plus(x, y) } -> std::convertible_to<T>;
        { multiplies(x, y) } -> std::convertible_to<T>;
    }
struct semiring_wrapper {
    using value_type = T;
    using plus = _Plus;
    using multiplies = _Multiplies;

    T data;

    semiring_wrapper() {
        static_assert(semiring<semiring_wrapper>);
    }

    semiring_wrapper(T data) : data(std::move(data)) {
        static_assert(semiring<semiring_wrapper>);
    }

    constexpr static semiring_wrapper zero() {
        return {_Zero};
    }

    constexpr static semiring_wrapper identity() {
        return {_Identity};
    }

    friend semiring_wrapper operator+(const semiring_wrapper &x, const semiring_wrapper &y) {
        return {_Plus{}(x.data, y.data)};
    }

    friend semiring_wrapper &operator+=(semiring_wrapper &x, const semiring_wrapper &y) {
        return x = x + y;
    }

    friend semiring_wrapper operator*(const semiring_wrapper &x, const semiring_wrapper &y) {
        return {_Multiplies{}(x.data, y.data)};
    }

    friend semiring_wrapper &operator*=(semiring_wrapper &x, const semiring_wrapper &y) {
        return x = x * y;
    }
};

// NOLINTEND(bugprone-reserved-identifier)

#endif //UNTITLED_ALGEBRA_WRAPPERS_H
