#ifndef XOR_WRAPPER_H
#define XOR_WRAPPER_H
#include <concepts>

template<typename T>
concept xor_able = requires(T x, T y)
{
    {x ^ y} -> std::same_as<T>;
    {x ^= y} -> std::same_as<T&>;
};

template<xor_able T>
struct xor_wrapper {
    T data;
};

template<xor_able T>
xor_wrapper<T> operator+(const xor_wrapper<T>& x, const xor_wrapper<T>& y) {
    return {x.data ^ y.data};
}

template<xor_able T>
xor_wrapper<T>& operator+=(xor_wrapper<T>& x, const xor_wrapper<T>& y) {
    return {x.data ^= y.data};
}

template<xor_able T>
xor_wrapper<T> operator-(const xor_wrapper<T>& x, const xor_wrapper<T>& y) {
    return {x.data ^ y.data};
}

template<xor_able T>
xor_wrapper<T> operator*(const size_t times, const xor_wrapper<T>& x) {
    return times & 1 ? x : T{};
}

#endif //XOR_WRAPPER_H
