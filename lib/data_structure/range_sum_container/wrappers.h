#ifndef WRAPPERS_H
#define WRAPPERS_H
#include <algorithm>
#include <concepts>
template<typename T>
concept max_able = requires(T x, T y)
{
    { std::max(x, y) } -> std::same_as<const T &>;
};

template<max_able T>
struct max_wrapper {
    T data;

    max_wrapper() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    max_wrapper(T data): data(data) {
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator T() const {
        return data;
    }
};

template<max_able T>
max_wrapper<T> operator+(const max_wrapper<T> x, const max_wrapper<T> y) {
    return {std::max(x.data, y.data)};
}

template<max_able T>
max_wrapper<T> &operator+=(max_wrapper<T> &x, const max_wrapper<T> y) {
    x.data = std::max(x.data, y.data);
    return x;
}

template<typename T>
concept min_able = requires(T x, T y)
{
    { std::min(x, y) } -> std::same_as<const T &>;
};

template<min_able T>
struct min_wrapper {
    T data;

    min_wrapper() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    min_wrapper(T data): data(data) {
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator T() const {
        return data;
    }
};

template<min_able T>
min_wrapper<T> operator+(const min_wrapper<T> x, const min_wrapper<T> y) {
    return {std::min(x.data, y.data)};
}

template<min_able T>
min_wrapper<T> &operator+=(min_wrapper<T> &x, const min_wrapper<T> y) {
    x.data = std::min(x.data, y.data);
    return x;
}

template<typename T>
concept xor_able = requires(T x, T y)
{
    { x ^ y } -> std::same_as<T>;
    { x ^= y } -> std::same_as<T &>;
};

template<xor_able T>
struct xor_wrapper {
    T data;
};

template<xor_able T>
xor_wrapper<T> operator+(const xor_wrapper<T> &x) {
    return x;
}

template<xor_able T>
xor_wrapper<T> operator-(const xor_wrapper<T> &x) {
    return x;
}

template<xor_able T>
xor_wrapper<T> operator+(const xor_wrapper<T> &x, const xor_wrapper<T> &y) {
    return {x.data ^ y.data};
}

template<xor_able T>
xor_wrapper<T> &operator+=(xor_wrapper<T> &x, const xor_wrapper<T> &y) {
    x.data ^= y.data;
    return x;
}

template<xor_able T>
xor_wrapper<T> operator-(const xor_wrapper<T> &x, const xor_wrapper<T> &y) {
    return {x.data ^ y.data};
}

template<xor_able T>
xor_wrapper<T> &operator-=(xor_wrapper<T> &x, const xor_wrapper<T> &y) {
    x.data ^= y.data;
    return x;
}

template<xor_able T>
xor_wrapper<T> operator*(const size_t times, const xor_wrapper<T> &x) {
    return times & 1 ? x : T{};
}

#endif //WRAPPERS_H
