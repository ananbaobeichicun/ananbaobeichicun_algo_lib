// #define NDEBUG
#include <bits/stdc++.h>
#include "mod_int.h"
#include "matrix.h"
#include "simd_matrix.h"

using ull = unsigned long long;
def_mod_type(ull, 998244353)

namespace {
    struct timer {
        std::chrono::nanoseconds &n;
        std::chrono::time_point<std::chrono::steady_clock> c;

        explicit timer(std::chrono::nanoseconds &n) : n(n), c(std::chrono::steady_clock::now()) {
        }

        ~timer() {
            n += std::chrono::steady_clock::now() - c;
        }
    };
}

constexpr size_t TIMES = 100;

int main() {
    std::random_device rd;
    std::mt19937 gen{rd()};

    constexpr size_t m = 100, k = 100, n = 100;
    alignas(64) static std::array<ull, m * k> M;
    alignas(64) static std::array<ull, k * n> N;

    std::ranges::generate(M, [&] { return std::uniform_int_distribution<ull>(0, MOD - 1)(gen); });
    std::ranges::generate(N, [&] { return std::uniform_int_distribution<ull>(0, MOD - 1)(gen); });

    alignas(64) static array_matrix<mull, m, k> MM(from_raw, (mull *) M.data());
    alignas(64) static array_matrix<mull, k, n> NN(from_raw, (mull *) N.data());

    std::chrono::nanoseconds naive_mul_time{};
    array_matrix<mull, m, n> naive_mul_ans; // NOLINT(*-pro-type-member-init)
    for (size_t i = 0; i < TIMES; ++i) {
        timer t(naive_mul_time);
        naive_mul_ans = MM * NN;
    }
    std::println(std::cerr, "naive: {:>15}", naive_mul_time);

    constexpr montgomery_32 mont(MOD);
    alignas(64) static std::array<ull, m * n> simd_mul_ans{};
    std::chrono::nanoseconds simd_mul_time{};
    for (size_t i = 0; i < TIMES; ++i) {
        std::ranges::fill(simd_mul_ans, 0);
        {
            timer t(simd_mul_time);
            mont.batch_encode(M.data(), M.data(), m * k);
            mont.batch_encode(N.data(), N.data(), k * n);
            simd_matrix_impl::mul_montgomery_raw(M.data(), N.data(), simd_mul_ans.data(), m, k, n, mont);
            mont.batch_REDC(simd_mul_ans.data(), simd_mul_ans.data(), m * n);
        }

        mont.batch_REDC(M.data(), M.data(), m * k);
        mont.batch_REDC(N.data(), N.data(), k * n);
    }
    std::println(std::cerr, " simd: {:>15}", simd_mul_time);
    std::println(std::cerr, "ratio: {:>15}", (long double) simd_mul_time.count() / naive_mul_time.count());

    assert(std::ranges::equal((ull *) naive_mul_ans.data.data(), (ull *) naive_mul_ans.data.data() + m * n,
        simd_mul_ans.begin(), simd_mul_ans.end()));

    std::chrono::nanoseconds naive_pow_time{};
    array_matrix<mull, m, k> naive_pow_ans{};
    for (size_t i = 0; i < TIMES; ++i) {
        timer t(naive_pow_time);
        naive_pow_ans = MM.pow(~0uz);
    }
    std::println(std::cerr, "naive: {:>15}", naive_pow_time);

    std::chrono::nanoseconds simd_pow_time{};
    alignas(64) static std::array<ull, m * k> simd_pow_ans{};
    for (size_t i = 0; i < TIMES; ++i) {
        std::ranges::fill(simd_pow_ans, 0);
        {
            timer t(simd_pow_time);
            for (size_t j = 0; j < m; ++j)
                simd_pow_ans[j * k + j] = mont.REDC(mont.r2);
            mont.batch_encode(M.data(), M.data(), m * k);
            alignas(64) static std::array<ull, m * k> tmp; // NOLINT(*-pro-type-member-init)
            std::ranges::copy(M, tmp.begin());
            simd_matrix_impl::mod_pow_montgomery(tmp.data(), simd_pow_ans.data(), m, ~0uz, mont);
            mont.batch_REDC(simd_pow_ans.data(), simd_pow_ans.data(), m * k);
        }
        mont.batch_REDC(M.data(), M.data(), m * k);
    }
    std::println(std::cerr, " simd: {:>15}", simd_pow_time);
    std::println(std::cerr, "ratio: {:>15}", (long double) simd_pow_time.count() / naive_pow_time.count());

    assert(std::ranges::equal((ull *) naive_pow_ans.data.data(), (ull *) naive_pow_ans.data.data() + m * k,
        simd_pow_ans.begin(), simd_pow_ans.end()));
}
