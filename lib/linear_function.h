#ifndef LINEAR_FUNCTION_H
#define LINEAR_FUNCTION_H
#include <concepts>

template<std::floating_point T>
struct linear_function {
    T k, b;
    linear_function (): k(1.), b(0.) {
    }
};

template<typename T>
linear_function<T> operator+(linear_function<T> f, linear_function<T> g) {
    return {f.k * g.k, f.k * g.b + f.b};
}

template<typename T>
linear_function<T> operator-(linear_function<T> g) {
    return {1. / g.k, - g.b / g.k};
}

template<typename T>
linear_function<T> operator-(linear_function<T> f, linear_function<T> g) {
    return f + -g;
}

#endif //LINEAR_FUNCTION_H
