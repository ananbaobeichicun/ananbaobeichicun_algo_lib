#ifndef MATRIX_H
#define MATRIX_H
#include <algorithm>
#include <vector>
#include <concepts>
#include <ranges>
#include <cassert>
#include "concepts/algebra_concepts.h"
#include "number/montgomery_32.h"

#define DEFINE_ASSIGNMENT_OPERATOR(struct_name, op, enable_cond) \
friend struct_name &operator op##=(struct_name &x, const struct_name &y) \
requires (enable_cond) { \
    return x = x op y; \
}

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
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

    template<typename R, typename T>
    concept double_nested_range = std::ranges::forward_range<R>
                                  && std::ranges::forward_range<std::ranges::range_value_t<R> >
                                  && std::same_as<T, std::ranges::range_value_t<std::ranges::range_value_t<R> > >;

    template<typename M>
    concept matrix_like = requires
                          {
                              typename M::value_type;
                          } && semiring<typename M::value_type>
                          && requires(M m, const M cm, const typename M::value_type x, size_t r, size_t c)
                          {
                              { cm.rows() } -> std::convertible_to<size_t>;
                              { cm.cols() } -> std::convertible_to<size_t>;
                              { m[r, c] = x } -> std::convertible_to<typename M::value_type &>;
                              { cm[r, c] } -> std::convertible_to<const typename M::value_type &>;
                          };

    namespace matrix_impl {
        template<matrix_like M, typename Fn, matrix_like O>
            requires std::same_as<typename M::value_type, typename O::value_type>
                     && std::invocable<Fn, const typename M::value_type &>
                     && std::same_as<std::invoke_result_t<Fn, const typename M::value_type &>, typename M::value_type>
        void map(const M &m, Fn fn, O &ans) {
            for (size_t i = 0; i < m.rows(); ++i)
                for (size_t j = 0; j < m.cols(); ++j)
                    ans[i, j] = std::invoke(fn, m[i, j]);
        }

        template<matrix_like M, matrix_like N, typename Fn, matrix_like O>
            requires std::same_as<typename M::value_type, typename N::value_type>
                     && std::same_as<typename N::value_type, typename O::value_type>
                     && std::invocable<Fn, const typename M::value_type &, const typename M::value_type &>
                     && std::same_as<std::invoke_result_t<Fn, const typename M::value_type &, const typename
                         M::value_type
                         &>, typename M::value_type>
        void zip(const M &m, const N &n, Fn fn, O &ans) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            for (size_t i = 0; i < m.rows(); ++i)
                for (size_t j = 0; j < m.cols(); ++j)
                    ans[i, j] = std::invoke(fn, m[i, j], n[i, j]);
        }

        template<matrix_like M>
        void pos(const M &m, M &ans) {
            ans = m;
        }

        template<matrix_like M>
            requires ring<typename M::value_type>
        void neg(const M &m, M &ans) {
            map(m, [](const typename M::value_type &x) { return -x; }, ans);
        }

        template<matrix_like M, matrix_like N, matrix_like O>
            requires std::same_as<typename M::value_type, typename N::value_type>
                     && std::same_as<typename N::value_type, typename O::value_type>
        void add(const M &m, const N &n, O &ans) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            zip(m, n, std::plus<typename M::value_type>{}, ans);
        }

        template<matrix_like M, matrix_like N, matrix_like O>
            requires std::same_as<typename M::value_type, typename N::value_type>
                     && std::same_as<typename N::value_type, typename O::value_type>
                     && ring<typename M::value_type>
        void sub(const M &m, const N &n, O &ans) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            zip(m, n, std::minus<typename M::value_type>{}, ans);
        }

        template<matrix_like M, matrix_like N, matrix_like O>
            requires std::same_as<typename M::value_type, typename N::value_type>
                     && std::same_as<typename N::value_type, typename O::value_type>
        void mul(const M &m, const N &n, O &ans) {
            assert(m.cols() == n.rows());
            const size_t a = m.rows(), b = m.cols(), c = n.cols();
            for (size_t i = 0; i < a; ++i)
                for (size_t j = 0; j < b; ++j)
                    for (size_t k = 0; k < c; ++k)
                        ans[i, k] += m[i, j] * n[j, k];
        }

        template<matrix_like M>
        void scalar_left_mul(const typename M::value_type &scalar, const M &m, M &ans) {
            map(m, [&scalar](typename M::value_type num) { return scalar * num; }, ans);
        }

        template<matrix_like M>
        void scalar_right_mul(const M &m, const typename M::value_type &scalar, M &ans) {
            map(m, [&scalar](typename M::value_type num) { return num * scalar; }, ans);
        }

        template<matrix_like M, matrix_like N>
            requires std::same_as<typename M::value_type, typename N::value_type>
        M &add_assign(M &m, const N &n) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            for (size_t i = 0; i < m.rows(); ++i)
                for (size_t j = 0; j < m.cols(); ++j)
                    m[i, j] += n[i, j];
            return m;
        }

        template<matrix_like M, matrix_like N>
            requires std::same_as<typename M::value_type, typename N::value_type>
                     && ring<typename M::value_type>
        M &sub_assign(M &m, const N &n) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            for (size_t i = 0; i < m.rows(); ++i)
                for (size_t j = 0; j < m.cols(); ++j)
                    m[i, j] -= n[i, j];
            return m;
        }

        template<matrix_like M>
        void pow(M m, size_t t, M &ans) {
            assert(m.rows() == m.cols());
            for (; t; t >>= 1) {
                if (t & 1)
                    ans *= m;
                m *= m;
            }
        }

        template<matrix_like M, matrix_like N>
        bool equal(const M &m, const N &n) {
            assert(m.rows() == n.rows() && m.cols() == n.cols());
            for (size_t i = 0; i < n.rows(); ++i)
                for (size_t j = 0; j < n.cols(); ++j)
                    if (m[i, j] != n[i, j])
                        return false;
            return true;
        }
    }

    template<semiring T>
    struct vector_matrix {
        using value_type = T;
        size_t n, m;
        std::vector<std::vector<T> > data;

        vector_matrix(const size_t n, const size_t m, const T &value = T::zero()) : n(n), m(m),
            data(n, std::vector<T>(m, value)) {
        }

    private:
        template<double_nested_range<T> R>
        explicit vector_matrix(R r, int) : vector_matrix(std::ranges::distance(r),
                                                         std::ranges::empty(r)
                                                             ? 0
                                                             : std::ranges::distance(*std::ranges::begin(r))) {
            assert(inner_same_length(r));
            std::ranges::for_each(r, [&, it = data.begin()](const auto &rr) mutable {
                std::ranges::copy(rr, it++->begin());
            });
        }

    public:
        template<double_nested_range<T> R>
        explicit vector_matrix(R r) : vector_matrix(std::move(r), 0) {
        }

        vector_matrix(std::initializer_list<std::initializer_list<T> > list) : vector_matrix(std::move(list), 0) {
        }

        static vector_matrix scalar(const size_t n, const value_type &value = value_type::identity()) {
            vector_matrix ans(n, n);
            for (size_t i = 0; i < n; ++i)
                ans[i, i] = value;
            return ans;
        }

        [[nodiscard]] constexpr size_t rows() const noexcept {
            return n;
        }

        [[nodiscard]] constexpr size_t cols() const noexcept {
            return m;
        }

        T &operator[](const size_t i, const size_t j) {
            return data[i][j];
        }

        const T &operator[](const size_t i, const size_t j) const {
            return data[i][j];
        }

        template<typename Fn>
            requires std::invocable<Fn, const T &>
                     && std::same_as<std::invoke_result_t<Fn, const T &>, T>
        vector_matrix map(Fn fn) const {
            vector_matrix ans(n, m);
            matrix_impl::map(*this, fn, ans);
            return ans;
        }

        template<typename Fn>
            requires std::invocable<Fn, const T &, const T &>
                     && std::same_as<std::invoke_result_t<Fn, const T &, const T &>, T>
        static vector_matrix zip(const vector_matrix &x, const vector_matrix &y, Fn fn) {
            assert(x.rows() == y.rows() && x.cols() == y.cols());
            vector_matrix ans(x.rows(), x.cols());
            matrix_impl::zip(x, y, fn, ans);
            return ans;
        }

        vector_matrix operator+() const {
            vector_matrix ans(n, m);
            matrix_impl::pos(*this, ans);
            return ans;
        }

        vector_matrix operator-() const
            requires ring<T> {
            vector_matrix ans(n, m);
            matrix_impl::neg(*this, ans);
            return ans;
        }

        friend vector_matrix operator+(const vector_matrix &x, const vector_matrix &y) {
            assert(x.rows() == y.rows() && x.cols() == y.cols());
            vector_matrix ans(x.rows(), x.cols());
            matrix_impl::add(x, y, ans);
            return ans;
        }

        friend vector_matrix operator-(const vector_matrix &x, const vector_matrix &y)
            requires ring<T> {
            assert(x.rows() == y.rows() && x.cols() == y.cols());
            vector_matrix ans(x.rows(), x.cols());
            matrix_impl::sub(x, y, ans);
            return ans;
        }

        friend vector_matrix operator*(const vector_matrix &x, const vector_matrix &y) {
            assert(x.cols() == y.rows());
            vector_matrix ans(x.rows(), y.cols());
            matrix_impl::mul(x, y, ans);
            return ans;
        }

        friend vector_matrix operator*(const T &scalar, const vector_matrix &x) {
            vector_matrix ans(x.rows(), x.cols());
            matrix_impl::scalar_left_mul(scalar, x, ans);
            return ans;
        }

        friend vector_matrix operator*(const vector_matrix &x, const T &scalar) {
            vector_matrix ans(x.rows(), x.cols());
            matrix_impl::scalar_right_mul(x, scalar, ans);
            return ans;
        }

        vector_matrix &operator+=(const vector_matrix &other) {
            return matrix_impl::add_assign(*this, other);
        }

        vector_matrix &operator-=(const vector_matrix &other)
            requires ring<T> {
            return matrix_impl::sub_assign(*this, other);
        }

        DEFINE_ASSIGNMENT_OPERATOR(vector_matrix, *, true)

        vector_matrix pow(const size_t t) const {
            vector_matrix ans = vector_matrix::scalar(n);
            matrix_impl::pow(*this, t, ans);
            return ans;
        }

        friend bool operator==(const vector_matrix &x, const vector_matrix &y) {
            return matrix_impl::equal(x, y);
        }
    };

    struct from_raw_t {
    } from_raw;

    template<semiring T, size_t N, size_t M>
    struct array_matrix {
        using value_type = T;
        std::array<T, N * M> data;

        explicit array_matrix() = default;

        explicit array_matrix(from_raw_t, T *ptr) {
            std::copy_n(ptr, N * M, data.begin());
        }

        explicit array_matrix(const T &value) {
            data.fill(value);
        }

    private:
        template<double_nested_range<T> R>
        explicit array_matrix(R r, int) {
            assert(inner_same_length(r));
            assert(std::ranges::distance(r) == N);
            assert(std::ranges::empty(r) || std::ranges::distance(*std::ranges::begin(r)) == M);
            auto it = data.begin();
            for (const auto &row: r)
                it = std::ranges::copy(row, it).it;
        }

    public:
        template<double_nested_range<T> R>
        explicit array_matrix(R r) : array_matrix(std::move(r), 0) {
        }

        array_matrix(std::initializer_list<std::initializer_list<T> > list) : array_matrix(std::move(list), 0) {
        }

        static array_matrix scalar(const value_type &value = value_type::identity())
            requires (N == M) {
            array_matrix ans(T::zero());
            for (size_t i = 0; i < N; ++i)
                ans[i, i] = value;
            return ans;
        }

        [[nodiscard]] static constexpr size_t rows() noexcept {
            return N;
        }

        [[nodiscard]] static constexpr size_t cols() noexcept {
            return M;
        }

        T &operator[](const size_t i, const size_t j) {
            return data[i * M + j];
        }

        const T &operator[](const size_t i, const size_t j) const {
            return data[i * M + j];
        }

        template<typename Fn>
            requires std::invocable<Fn, const T &>
                     && std::same_as<std::invoke_result_t<Fn, const T &>, T>
        array_matrix map(Fn fn) const {
            array_matrix ans{T::zero()};
            matrix_impl::map(*this, fn, ans);
            return ans;
        }

        template<typename Fn>
            requires std::invocable<Fn, const T &, const T &>
                     && std::same_as<std::invoke_result_t<Fn, const T &, const T &>, T>
        static array_matrix zip(const array_matrix &x, const array_matrix &y, Fn fn) {
            array_matrix ans{T::zero()};
            matrix_impl::zip(x, y, fn, ans);
            return ans;
        }

        array_matrix operator+() const {
            array_matrix ans{T::zero()};
            matrix_impl::pos(*this, ans);
            return ans;
        }

        array_matrix operator-() const
            requires ring<T> {
            array_matrix ans{T::zero()};
            matrix_impl::neg(*this, ans);
            return ans;
        }

        friend array_matrix operator+(const array_matrix &x, const array_matrix &y) {
            array_matrix ans{T::zero()};
            matrix_impl::add(x, y, ans);
            return ans;
        }

        friend array_matrix operator-(const array_matrix &x, const array_matrix &y)
            requires ring<T> {
            array_matrix ans{T::zero()};
            matrix_impl::sub(x, y, ans);
            return ans;
        }

        friend array_matrix operator*(const T &scalar, const array_matrix &x) {
            array_matrix ans{T::zero()};
            matrix_impl::scalar_left_mul(scalar, x, ans);
            return ans;
        }

        friend array_matrix operator*(const array_matrix &x, const T &scalar) {
            array_matrix ans{T::zero()};
            matrix_impl::scalar_right_mul(x, scalar, ans);
            return ans;
        }

        array_matrix &operator+=(const array_matrix &other) {
            return matrix_impl::add_assign(*this, other);
        }

        array_matrix &operator-=(const array_matrix &other)
            requires ring<T> {
            return matrix_impl::sub_assign(*this, other);
        }

        DEFINE_ASSIGNMENT_OPERATOR(array_matrix, *, N == M)

        array_matrix pow(const size_t n) const
            requires (N == M) {
            array_matrix ans = array_matrix::scalar();
            matrix_impl::pow(*this, n, ans);
            return ans;
        }

        friend bool operator==(const array_matrix &x, const array_matrix &y) {
            return matrix_impl::equal(x, y);
        }
    };

    template<semiring T, size_t P, size_t Q, size_t R>
    array_matrix<T, P, R> operator*(const array_matrix<T, P, Q> &x, const array_matrix<T, Q, R> &y) {
        array_matrix<T, P, R> ans{T::zero()};
        matrix_impl::mul(x, y, ans);
        return ans;
    }
}

#undef DEFINE_ASSIGNMENT_OPERATOR

#endif //MATRIX_H
