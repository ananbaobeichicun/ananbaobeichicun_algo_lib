#include <bits/stdc++.h>
#include "../third_party/nanobench.h"
#include "simd_linear_base_64.h"
#include "linear_base.h"

constexpr size_t n = 10000;

int main() {
    std::array<uint64_t, 10000> arr; // NOLINT(*-pro-type-member-init)
    std::array<uint64_t, n> queries; // NOLINT(*-pro-type-member-init)
    std::random_device rd;
    std::mt19937_64 gen{rd()};

    std::ranges::generate(arr, std::ref(gen));
    std::ranges::generate(queries, std::ref(gen));

    std::array<uint64_t, n> naive_ans; // NOLINT(*-pro-type-member-init)
    ankerl::nanobench::Bench().run("naive", [&] {
        linear_base<uint64_t> lb;
        for (const uint64_t x: arr)
            lb.insert(x);
        for (size_t i = 0; i < n; ++i)
            naive_ans[i] = lb.eliminate(queries[i]);
    });

    std::array<uint64_t, n> simd_ans; // NOLINT(*-pro-type-member-init)
    ankerl::nanobench::Bench().run("simd", [&] {
        simd_linear_base_64 simd_lb;
        for (const uint64_t x: arr)
            simd_lb.insert(x);
        for (size_t i = 0; i < n; ++i)
            simd_ans[i] = simd_lb.eliminate(queries[i]);
    });

    assert(std::ranges::equal(naive_ans, simd_ans));
}
