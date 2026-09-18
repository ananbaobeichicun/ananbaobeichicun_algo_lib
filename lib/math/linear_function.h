#ifndef LINEAR_FUNCTION_H
#define LINEAR_FUNCTION_H
#include <concepts>

template<typename T>
struct linear_function {
    T k, b;

    linear_function() : k(1), b(0) {
    }

    linear_function inv() const {
        return {1 / k, -b / k};
    }
};

template<typename T>
linear_function<T> operator*(linear_function<T> f, linear_function<T> g) {
    return {f.k * g.k, f.k * g.b + f.b};
}

#endif //LINEAR_FUNCTION_H
