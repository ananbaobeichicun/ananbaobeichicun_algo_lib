#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <array>
#include <functional>

template<std::integral T, size_t height, size_t width>
class matrix {
public:
    std::array<std::array<T, width>, height> data;

    explicit matrix(const T initial = 0) {
        for (auto &i: data)
            for (auto &j: i)
                j = initial;
    }

    matrix(const std::initializer_list<std::initializer_list<T> > &lis) {
        for (size_t i = 0; i < height; ++i)
            for (size_t j = 0; j < width; ++j)
                data[i][j] = lis[i][j];
    }

    matrix(std::initializer_list<std::initializer_list<T> > &&lis) {
        for (auto i = lis.begin(); i < lis.begin() + height; ++i)
            for (auto j = i->begin(); j < i->begin() + width; ++j)
                data[i - lis.begin()][j - i->begin()] = std::move(*j);
    }

    matrix(const std::initializer_list<std::array<T, width> > &lis) {
        for (size_t i = 0; i < height; ++i)
            data[i] = std::data(lis)[i];
    }

    matrix(std::initializer_list<std::vector<T> > &&lis) {
        for (size_t i = 0; i < height; ++i)
            data[i] = std::move(lis[i]);
    }

    explicit matrix(const std::array<std::array<T, width>, height> &lis) : data(lis) {
    }

    explicit matrix(std::array<std::array<T, width>, height> &&lis): data(std::move(lis)) {
    }

    std::array<T, width> &operator[](size_t i) {
        return data[i];
    }

    const std::array<T, width> &operator[](size_t i) const {
        return data[i];
    }

    template<typename Fn>
        requires std::invocable<Fn, T>
    matrix map(Fn fun) const {
        matrix ans;
        for (size_t i = 0; i < height; ++i)
            for (size_t j = 0; j < width; ++j)
                ans.data[i][j] = fun(data[i][j]);
        return ans;
    }
};

template<std::integral T, size_t a>
class square_matrix : public matrix<T, a, a> {
public:
    using matrix<T, a, a>::matrix;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    square_matrix(const matrix<T, a, a> &m) : matrix<T, a, a>(m) {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    square_matrix(matrix<T, a, a> &&m) noexcept: matrix<T, a, a>(std::move(m)) {
    }

    square_matrix map(std::function<T(T)> fun) const {
        square_matrix ans;
        for (size_t i = 0; i < a; ++i)
            for (size_t j = 0; j < a; ++j)
                ans[i][j] = fun((*this)[i][j]);
        return ans;
    }

    static constexpr square_matrix identity() {
        square_matrix ans;
        for (size_t i = 0; i < a; ++i)
            ans[i][i] = 1;
        return ans;
    }
};

template<std::integral T, size_t height, size_t width>
matrix<T, height, width> zip(const matrix<T, height, width> &x, const matrix<T, height, width> &y,
                             std::function<T(T, T)> fn) {
    matrix<T, height, width> ans;
    for (size_t i = 0; i < height; ++i)
        for (size_t j = 0; j < width; ++j)
            ans[i][j] = fn(x[i][j], y[i][j]);
    return ans;
}

template<std::integral T, size_t height, size_t width>
matrix<T, height, width> operator+(const matrix<T, height, width> &x, const matrix<T, height, width> &y) {
    return zip(x, y, std::plus<T>{});
}

template<std::integral T, size_t height, size_t width>
matrix<T, height, width> operator-(const matrix<T, height, width> &x, const matrix<T, height, width> &y) {
    return zip(x, y, std::minus<T>{});
}

template<std::integral T, size_t a, size_t b, size_t c>
matrix<T, a, c> operator*(const matrix<T, a, b> &x, const matrix<T, b, c> &y) {
    matrix<T, a, c> ans;
    for (size_t i = 0; i < a; ++i)
        for (size_t j = 0; j < c; ++j)
            for (size_t k = 0; k < b; ++k)
                ans[i][j] += x[i][k] * y[k][j];
    return ans;
}

template<std::integral T, size_t a, size_t b, size_t c>
matrix<T, a, c> mod_times(const matrix<T, a, b> &x, const matrix<T, b, c> &y, T mod) {
    matrix<T, a, c> ans;
    for (size_t i = 0; i < a; ++i)
        for (size_t j = 0; j < c; ++j)
            for (size_t k = 0; k < b; ++k)
                ans[i][j] = (ans[i][j] + x[i][k] * y[k][j] % mod) % mod;
    return ans;
}

template<std::integral T, size_t a>
square_matrix<T, a> operator*(const square_matrix<T, a> &x, const square_matrix<T, a> &y) {
    return x * y;
}

template<std::integral T, size_t a>
square_matrix<T, a> mod_times(const square_matrix<T, a> &x, const square_matrix<T, a> &y, T mod) {
    return mod_times<T, a, a, a>(x, y, mod);
}

template<std::integral T, size_t a>
square_matrix<T, a> operator%(const square_matrix<T, a> &x, T mod) {
    return x.map([mod](T num) { return num % mod; });
}

template<std::integral T, size_t a>
square_matrix<T, a> operator*(const square_matrix<T, a> &x, T scale) {
    return x.map([scale](T num) { return num * scale; });
}

// using ull = unsigned long long;
//
// template<typename T, size_t a>
// square_matrix<T, a> matrix_fast_mod_pow(square_matrix<T, a> m, ull b, T mod) {
//     auto ans = square_matrix<T, a>::identity();
//     for (; b; b >>= 1) {
//         if (b & 1)
//             ans = mod_times(ans, m, mod);
//         m = mod_times(m, m, mod);
//     }
//     return ans;
// }

#endif //MATRIX_H
