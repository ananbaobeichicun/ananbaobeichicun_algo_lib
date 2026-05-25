#ifndef MATRIX_H
#define MATRIX_H
#include <algorithm>
#include <vector>
#include <concepts>
#include <ranges>
#include <cassert>

#include "concepts/algebra_concepts.h"

template<std::ranges::forward_range R>
    requires std::ranges::forward_range<std::ranges::range_value_t<R> >
bool inner_same_length(const R &r) {
    auto beg = std::ranges::begin(r);
    auto end = std::ranges::end(r);

    if (beg == end)
        return true;

    auto expected_len = std::ranges::distance(*beg);

    return std::ranges::all_of(r, [&](const std::ranges::range_value_t<R> &rr) {
        return std::ranges::distance(rr) == expected_len;
    });
}

template<semiring T>
struct matrix {
    using value_type = T;

    size_t n, m;
    std::vector<std::vector<T> > data;

    explicit matrix(const size_t n, const size_t m) : n(n), m(m), data(n, std::vector<T>(m)) {
        for (auto &i: data)
            for (auto &j: i)
                j = T::zero();
    }

    static matrix scalar(const size_t n, const T &x = T::identity()) {
        matrix ans(n, n);
        for (size_t i = 0; i < n; ++i)
            ans.data[i][i] = x;
        return ans;
    }

    explicit matrix(std::vector<std::vector<T> > r) : n(r.size()), m(r.empty() ? 0 : r[0].size()), data(std::move(r)) {
        assert(inner_same_length(r));
    }

    template<std::ranges::forward_range R>
        requires std::ranges::forward_range<std::ranges::range_value_t<R> >
                 && std::same_as<T, std::ranges::range_value_t<std::ranges::range_value_t<R> > >
    explicit matrix(R r) : matrix(std::ranges::distance(r),
                                  std::ranges::empty(r) ? 0 : std::ranges::distance(*std::ranges::begin(r))) {
        assert(inner_same_length(r));
        std::ranges::for_each(r, [&, it = data.begin()](const auto &rr) mutable {
            std::ranges::copy(rr, it++->begin());
        });
    }

    matrix(std::initializer_list<std::initializer_list<T> > list)
        : matrix(list.size(), std::ranges::empty(list) ? 0 : list.begin()->size()) {
        assert(inner_same_length(list));
        auto it = data.begin();
        for (const auto &row: list)
            std::ranges::copy(row, it++->begin());
    }

    const std::vector<T> &operator[](const size_t i) const {
        return data[i];
    }

    std::vector<T> &operator[](const size_t i) {
        return data[i];
    }

    T &operator[](const size_t i, const size_t j) {
        return data[i][j];
    }

    const T &operator[](const size_t i, const size_t j) const {
        return data[i][j];
    }

    template<typename Fn>
        requires std::invocable<Fn, const T &> && std::same_as<std::invoke_result_t<Fn, const T &>, T>
    matrix map(Fn fun) const {
        matrix ans(n, m);
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < m; ++j)
                ans.data[i][j] = fun(data[i][j]);
        return ans;
    }

    template<typename Fn>
        requires std::invocable<Fn, const T &, const T &>
                 && std::same_as<std::invoke_result_t<Fn, const T &, const T &>, T>
    friend matrix zip(const matrix &x, const matrix &y, Fn fn) {
        assert(x.n == y.n && x.m == y.m);
        matrix ans(x.n, x.m);
        for (size_t i = 0; i < x.n; ++i)
            for (size_t j = 0; j < x.m; ++j)
                ans[i][j] = fn(x[i][j], y[i][j]);
        return ans;
    }

    friend matrix operator+(const matrix &x) {
        return x;
    }

    template<ring U>
    friend matrix<U> operator-(const matrix<U> &x) {
        return x.map([](const T &xx) { return -xx; });
    }

    friend matrix operator+(const matrix &x, const matrix &y) {
        assert(x.n == y.n && x.m == y.m);
        return zip(x, y, std::plus<T>{});
    }

    friend matrix &operator+=(matrix &x, const matrix &y) {
        return x = x + y;
    }

    template<ring U>
    friend matrix<U> operator-(const matrix<U> &x, const matrix<U> &y) {
        assert(x.n == y.n && x.m == y.m);
        return zip(x, y, std::minus<U>{});
    }

    friend matrix &operator-=(matrix &x, const matrix &y) {
        return x = x + y;
    }

    friend matrix operator*(const matrix &x, const matrix &y) {
        assert(x.m == y.n);

        const size_t a = x.n, b = x.m, c = y.m;
        matrix ans(x.n, y.m);
        for (size_t i = 0; i < a; ++i)
            for (size_t j = 0; j < c; ++j)
                for (size_t k = 0; k < b; ++k)
                    ans[i][j] += x[i][k] * y[k][j];
        return ans;
    }

    friend matrix operator*(const T &scale, const matrix &x) {
        return x.map([scale](T num) { return scale * num; });
    }

    friend matrix &operator*=(matrix &x, const matrix &y) {
        return x = x * y;
    }
};

#endif //MATRIX_H
