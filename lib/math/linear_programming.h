#ifndef LINEAR_PROGRAMMING_H
#define LINEAR_PROGRAMMING_H
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

namespace {
    /*
     * maximize c^T x
     * Ax == b
     * x >= 0
     */

    enum struct linear_programming_result_type { infeasible, unbounded, bounded };

    template<std::floating_point T>
    struct linear_programming_result {
        linear_programming_result_type type;
        std::vector<T> xs;
        T f;
    };

    template<std::floating_point T>
    void pivot(std::vector<std::vector<T> > &A, std::vector<T> &b, std::vector<T> &c, std::vector<size_t> &idx,
               const size_t in, const size_t out) {
        const size_t m = b.size(), n = c.size() - m;

        std::swap(idx[in], idx[out]);
        std::swap(c[in], c[out]);
        for (size_t i = 0; i < m; ++i)
            std::swap(A[i][in], A[i][out]);
        const T AA = A[out][out];
        b[out] /= AA;
        for (size_t i = 0; i < m + n; ++i)
            A[out][i] /= AA;

        for (size_t i = 0; i < m; ++i) {
            if (i == out)
                continue;
            b[i] -= A[i][out] * b[out];
            for (size_t j = 0; j < n + m; ++j)
                if (j != out)
                    A[i][j] -= A[i][out] * A[out][j];
            A[i][out] = 0;
        }
    }

    template<std::floating_point T>
    linear_programming_result_type raw_linear_programming(std::vector<std::vector<T> > &A, std::vector<T> &b,
                                                          std::vector<T> &c, std::vector<size_t> &idx, const T eps) {
        assert(eps > 0);
        const size_t m = b.size();
        assert(c.size() >= m);
        const size_t n = c.size() - m;

        while (true) {
            std::vector sigma(c);
            for (size_t i = 0; i < m; ++i)
                for (size_t j = m; j < m + n; ++j)
                    sigma[j] -= c[i] * A[i][j];

            T max_sigma = -std::numeric_limits<T>::infinity();
            size_t to_in = ~0uz;
            for (size_t i = m; i < m + n; ++i) {
                if (sigma[i] - max_sigma > eps || (std::abs(sigma[i] - max_sigma) < eps && idx[to_in] > idx[i])) {
                    to_in = i;
                    max_sigma = sigma[i];
                }
            }

            if (max_sigma <= eps)
                return linear_programming_result_type::bounded;

            T theta = std::numeric_limits<T>::infinity();
            size_t to_out = ~0uz;
            for (size_t i = 0; i < m; ++i) {
                if (A[i][to_in] > eps && (theta - b[i] / A[i][to_in] > eps
                                          || (std::abs(theta - b[i] / A[i][to_in]) < eps && idx[to_out] > idx[i]))) {
                    theta = b[i] / A[i][to_in];
                    to_out = i;
                }
            }

            if (!~to_out)
                return linear_programming_result_type::unbounded;

            pivot(A, b, c, idx, to_in, to_out);
        }
    }

    template<std::floating_point T>
    linear_programming_result<T> linear_programming(const std::vector<std::vector<T> > &A, std::vector<T> b,
                                                    const std::vector<T> &c, const T eps) {
        assert(eps > 0);
        size_t m = b.size(), n = c.size();
        assert(m <= n);
        assert(A.size() == m);
        std::vector A_(m, std::vector<T>(m + n));
        for (size_t i = 0; i < m; ++i) {
            assert(A[i].size() == n);
            std::ranges::copy(A[i], A_[i].begin() + m);
            if (b[i] < 0) {
                for (size_t j = m; j < m + n; ++j)
                    A_[i][j] = -A_[i][j];
                b[i] = -b[i];
            }

            A_[i][i] = 1;
        }

        std::vector<T> dummy_c(m + n);
        std::fill_n(dummy_c.begin(), m, -1);

        std::vector<size_t> idx(m + n);
        std::iota(idx.begin() + m, idx.end(), 0uz);
        std::iota(idx.begin(), idx.begin() + m, n);

        const linear_programming_result_type phase1_res = raw_linear_programming(A_, b, dummy_c, idx, eps);
        assert(phase1_res == linear_programming_result_type::bounded);

        T infeasible_acc = 0;
        for (size_t i = 0; i < m; ++i)
            if (idx[i] >= n)
                infeasible_acc += b[i];

        if (infeasible_acc > eps)
            return {linear_programming_result_type::infeasible, {}, -std::numeric_limits<T>::infinity()};

        std::vector<T> c_(m + n);
        for (size_t i = 0; i < m + n; ++i)
            if (idx[i] < n)
                c_[i] = c[idx[i]];

        for (size_t i = n + m - 1; ~i; --i) {
            if (idx[i] >= n) {
                size_t to_erase = i;
                if (i < m) {
                    auto it = std::find_if(A_[i].begin() + m, A_[i].end(), [eps](const T a) {
                        return std::abs(a) > eps;
                    });
                    if (it == A_[i].end()) {
                        A_.erase(A_.begin() + i);
                        b.erase(b.begin() + i);
                        --m;
                    } else {
                        pivot(A_, b, c_, idx, size_t(it - A_[i].begin()), i);
                        to_erase = it - A_[i].begin();
                    }
                }
                std::swap(c_[to_erase], c_[idx.size() - 1]);
                std::swap(idx[to_erase], idx[idx.size() - 1]);
                for (size_t j = 0; j < m; ++j) {
                    std::swap(A_[j][to_erase], A_[j][idx.size() - 1]);
                    A_[j].pop_back();
                }
                c_.pop_back();
                idx.pop_back();
            }
        }

        assert(A_.size() == m);
        assert(b.size() == m);
        assert(c_.size() == n);
        assert(idx.size() == n);

        for (const auto &row: A_)
            assert(row.size() == n);

        assert(std::ranges::all_of(idx, [n](const size_t id) { return id < n; }));

        const linear_programming_result_type ress = raw_linear_programming(A_, b, c_, idx, eps);
        if (ress == linear_programming_result_type::unbounded)
            return {linear_programming_result_type::unbounded, {}, std::numeric_limits<T>::infinity()};

        std::vector<T> xs(n);
        for (size_t i = 0; i < m; ++i)
            if (idx[i] < n)
                xs[idx[i]] = b[i];

        const T f = std::inner_product(c.begin(), c.begin() + n, xs.begin(), T{});
        return {linear_programming_result_type::bounded, xs, f};
    }
}

#endif //LINEAR_PROGRAMMING_H
