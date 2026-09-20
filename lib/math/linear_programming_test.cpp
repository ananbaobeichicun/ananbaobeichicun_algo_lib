#include <bits/stdc++.h>
#include <cassert>
#include "linear_programming.h"

using ld = long double;
constexpr ld eps = 1e-9l, inf = std::numeric_limits<ld>::infinity();

namespace {
    template<std::floating_point T>
    struct test_case {
        std::vector<std::vector<ld> > A;
        std::vector<ld> b, c;

        linear_programming_result<T> ans;

        [[nodiscard]] bool verify() const {
            linear_programming_result<T> res = linear_programming(A, b, c, eps);
            if (res.type != ans.type)
                return false;
            // for (size_t i = 0; i < c.size(); ++i)
            //     if (std::abs(res.xs[i] - ans.xs[i]) >= eps)
            //         return false;
            return res.f == ans.f || std::abs(res.f - ans.f) < eps;
        }
    };
}

static std::vector<test_case<ld> > cases{
    {
        .A = {{0}}, .b = {0}, .c = {0},
        .ans = {linear_programming_result_type::bounded, {}, 0}
    },
    {
        .A = {{0}}, .b = {1}, .c = {1},
        .ans = {linear_programming_result_type::infeasible, {}, -inf},
    },
    {
        .A = {{1}}, .b = {1}, .c = {1},
        .ans = {linear_programming_result_type::bounded, {}, 1},
    },
    {
        .A = {{1, -1}}, .b = {0}, .c = {1, 0},
        .ans = {linear_programming_result_type::unbounded, {}, inf},
    },
    {
        .A = {{-1}}, .b = {-1}, .c = {1},
        .ans = {linear_programming_result_type::bounded, {}, 1},
    },
    {
        .A = {}, .b = {}, .c = {1},
        .ans = {linear_programming_result_type::unbounded, {}, inf},
    },
};

int main() {
    for (const auto &item: cases)
        assert(item.verify());
}
