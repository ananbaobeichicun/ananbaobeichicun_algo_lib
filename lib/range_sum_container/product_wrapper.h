#ifndef PRODUCT_WRAPPER_H
#define PRODUCT_WRAPPER_H
#include <concepts>
template<typename T>
concept product_able = requires(T x, T y)
{
    {x * y} -> std::same_as<T>;
    {x *= y} -> std::same_as<T&>;
    {x / y} -> std::same_as<T>;
};

template<product_able T>
struct product_wrapper {
    T data;
};

template<product_able T>
product_wrapper<T> operator+(product_wrapper<T> x, product_wrapper<T> y) {
    return {x.data * y.data};
}

template<product_able T>
product_wrapper<T>& operator+=(product_wrapper<T>& x, product_wrapper<T> y) {
    return {x.data *= y.data};
}

template<product_able T>
product_wrapper<T> operator-(product_wrapper<T> x, product_wrapper<T> y) {
    return {x.data / y.data};
}

template<product_able T>
product_wrapper<T> operator*(const size_t times, const product_wrapper<T>& x) {
    if (!times)
        return T{};
    T res = (times >> 1) * x;
    return res * res * ((times & 1) * x);
}

#endif //PRODUCT_WRAPPER_H
