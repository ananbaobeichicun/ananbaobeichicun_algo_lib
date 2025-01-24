#ifndef MAX_WRAPPER_H
#define MAX_WRAPPER_H
#include <algorithm>
#include <concepts>
template<typename T>
concept max_able = requires(T x, T y)
{
    { std::max(x, y) } -> std::same_as<const T&>;
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
#endif //MAX_WRAPPER_H
