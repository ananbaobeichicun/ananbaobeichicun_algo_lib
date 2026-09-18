#ifndef COMPETITIVE_PROGRAMMING_LINEAR_PROGRAMMING_H
#define COMPETITIVE_PROGRAMMING_LINEAR_PROGRAMMING_H
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

    template<typename T>
    struct linear_programming_result {
        linear_programming_result_type type;
        std::vector<T> xs;
        T f;
    };

    template<typename T>
    linear_programming_result_type raw_linear_programming(std::vector<std::vector<T> > &A, std::vector<T> &b,
                                                          std::vector<T> &c, std::vector<size_t> &idx) {
        const size_t m = b.size(), n = c.size() - m;

        while (true) {
            std::vector sigma(c);
            for (size_t i = 0; i < m; ++i)
                for (size_t j = m; j < m + n; ++j)
                    sigma[j] -= c[i] * A[i][j];

            const size_t to_in = size_t(std::max_element(sigma.begin() + m, sigma.end()) - sigma.begin());

            if (sigma[to_in] <= 0)
                return linear_programming_result_type::bounded;

            T theta = std::numeric_limits<T>::infinity();
            size_t to_out = ~0uz;
            for (size_t i = 0; i < m; ++i) {
                if (A[i][to_in] > 0 && theta > b[i] / A[i][to_in]) {
                    theta = b[i] / A[i][to_in];
                    to_out = i;
                }
            }

            if (!~to_out)
                return linear_programming_result_type::unbounded;

            std::swap(idx[to_in], idx[to_out]);
            std::swap(c[to_in], c[to_out]);
            for (size_t i = 0; i < m; ++i)
                std::swap(A[i][to_in], A[i][to_out]);
            const T AA = A[to_out][to_out];
            b[to_out] /= AA;
            for (size_t i = 0; i < m + n; ++i)
                A[to_out][i] /= AA;

            for (size_t i = 0; i < m; ++i) {
                if (i == to_out)
                    continue;
                b[i] -= A[i][to_out] * b[to_out];
                for (size_t j = 0; j < n + m; ++j)
                    if (j != to_out)
                        A[i][j] -= A[i][to_out] * A[to_out][j];
                A[i][to_out] = 0;
            }
        }
    }

    template<typename T>
    linear_programming_result<T> linear_programming(const std::vector<std::vector<T> > &A, std::vector<T> b,
                                                 const std::vector<T> &c, const T eps) {
        const size_t m = b.size(), n = c.size();
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

        const linear_programming_result_type res = raw_linear_programming(A_, b, dummy_c, idx);
        assert(res == linear_programming_result_type::bounded);

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
                std::swap(c_[i], c_[idx.size() - 1]);
                std::swap(idx[i], idx[idx.size() - 1]);
                for (size_t j = 0; j < m; ++j) {
                    std::swap(A_[j][i], A_[j][idx.size() - 1]);
                    A_[j].pop_back();
                }
                c_.pop_back();
                idx.pop_back();
            }
        }

        for (size_t i = 0; i < m; ++i)
            A_[i].resize(n);

        const linear_programming_result_type ress = raw_linear_programming(A_, b, c_, idx);
        if (ress == linear_programming_result_type::unbounded)
            return {linear_programming_result_type::unbounded, {}, std::numeric_limits<T>::infinity()};

        std::vector<T> xs(n);
        for (size_t i = 0; i < m; ++i)
            if (idx[i] < n)
                xs[idx[i]] = b[i];

        const T f = std::inner_product(c.begin(), c.begin() + m, xs.begin(), T{});
        return {linear_programming_result_type::bounded, xs, f};
    }
}

#endif //COMPETITIVE_PROGRAMMING_LINEAR_PROGRAMMING_H
