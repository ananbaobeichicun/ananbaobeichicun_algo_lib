#ifndef SIMD_LINEAR_BASE_64_H
#define SIMD_LINEAR_BASE_64_H
#include <cassert>
#include <array>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <immintrin.h>
#pragma GCC push_options
#pragma GCC target("sse3", "sse2", "sse", "sse4", "sse4.1", "sse4.2", "ssse3", "avx2", "avx512f", "avx512dq", "avx512vl")
#pragma GCC target("bmi2")

struct simd_linear_base_64 {
    constexpr static size_t SIZE = std::numeric_limits<uint64_t>::digits;
    constexpr static size_t FIT_CNT = 512 / SIZE;
    constexpr static size_t CHUNK_CNT = SIZE / FIT_CNT;
    std::array<__m512i, CHUNK_CNT> data{};

    struct reference {
        size_t idx;
        simd_linear_base_64 &master;

        // ReSharper disable once CppNonExplicitConversionOperator
        operator uint64_t() const {
            return (uint64_t) master.data[idx / FIT_CNT][idx % FIT_CNT];
        }

        reference &operator=(const uint64_t val) {
            master.data[idx / FIT_CNT][idx % FIT_CNT] = (int32_t) val;
            return *this;
        }

        reference &operator=(const reference &other) {
            return *this = uint64_t(other);
        }
    };

    reference operator[](const size_t idx) {
        assert(idx < SIZE);
        return {.idx = idx, .master = *this};
    }

    uint64_t operator[](const size_t idx) const {
        assert(idx < SIZE);
        return (uint64_t) data[idx / FIT_CNT][idx % FIT_CNT];
    }

    void clear() {
        std::ranges::fill(data, __m512i{});
    }

    [[gnu::always_inline]] static uint64_t horizontal_xor_epi64(__m512i v) {
        v = _mm512_xor_si512(v, _mm512_alignr_epi64(_mm512_setzero_si512(), v, 4));
        v = _mm512_xor_si512(v, _mm512_alignr_epi64(_mm512_setzero_si512(), v, 2));
        v = _mm512_xor_si512(v, _mm512_alignr_epi64(_mm512_setzero_si512(), v, 1));
        return (uint64_t) v[0];
    }

    [[gnu::always_inline]] static uint64_t xor_all(const std::array<__m512i, 8> &arr) {
        __m512i acc = _mm512_setzero_si512();
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            acc = _mm512_xor_si512(acc, arr[i]);
        return horizontal_xor_epi64(acc);
    }

    [[nodiscard]] uint64_t eliminate(const uint64_t x) const {
        std::array<__mmask8, CHUNK_CNT> masks; // NOLINT(*-pro-type-member-init)
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            masks[i] = __mmask8(x >> (i * FIT_CNT) & ((1 << FIT_CNT) - 1));
        std::array<__m512i, CHUNK_CNT> tmp; // NOLINT(*-pro-type-member-init)
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            tmp[i] = _mm512_maskz_mov_epi64(masks[i], data[i]);

        return x ^ xor_all(tmp);
    }

    size_t insert(uint64_t x) {
        x = eliminate(x);
        if (!x)
            return ~0uz;
        const size_t idx = (size_t) std::bit_width(x) - 1;
        const __m512i chunk = _mm512_set1_epi64(int64_t(1uz << idx));

        std::array<__mmask8, 8> masks; // NOLINT(*-pro-type-member-init)
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            masks[i] = _mm512_cmpneq_epi64_mask(data[i] & chunk, _mm512_setzero_si512());
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            data[i] = _mm512_mask_xor_epi64(data[i], masks[i], data[i], _mm512_set1_epi64(int64_t(x)));

        data[idx / CHUNK_CNT] = _mm512_mask_set1_epi64( data[idx / CHUNK_CNT], __mmask8(1 << idx % CHUNK_CNT), int64_t(x));

        return idx;
    }

    [[nodiscard]] uint64_t query_max() const {
        return xor_all(data);
    }

    [[nodiscard]] bool empty() const {
        return !query_max();
    }

    [[nodiscard]] uint64_t compress() const {
        uint64_t ans = 0;
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            ans = (uint64_t) _mm512_cmpneq_epi64_mask(data[i], _mm512_setzero_si512()) << (i * FIT_CNT);
        return ans;
    }

    [[nodiscard]] uint64_t query_kth_min(const size_t k) const {
        const size_t compressed = compress();
        const uint64_t deposited = _pdep_u64(k, compressed);

        std::array<__m512i, CHUNK_CNT> tmp; // NOLINT(*-pro-type-member-init)
#pragma GCC unroll(CHUNK_CNT)
        for (size_t i = 0; i < CHUNK_CNT; ++i)
            tmp[i] = _mm512_maskz_mov_epi64(__mmask8(deposited >> (i * FIT_CNT)), data[i]);

        return xor_all(tmp);
    }

    [[nodiscard]] size_t size() const {
        return (size_t) std::popcount(compress());
    }

    void merge(const simd_linear_base_64 &other) {
#pragma GCC unroll(SIZE)
        for (size_t i = 0; i < SIZE; ++i)
            insert(other[i]);
    }

    [[nodiscard]] bool is_ge(const simd_linear_base_64 &other) const {
        for (size_t i = 0; i < SIZE; ++i)
            if (eliminate(other[i]))
                return false;
        return true;
    }

    friend std::partial_ordering operator<=>(const simd_linear_base_64 &x, const simd_linear_base_64 &y) {
        const bool res1 = x.is_ge(y), res2 = y.is_ge(x);
        return res1
                   ? res2
                         ? std::partial_ordering::equivalent
                         : std::partial_ordering::greater
                   : res2
                         ? std::partial_ordering::less
                         : std::partial_ordering::unordered;
    }

    // todo min_above max_below hash iterator
};

#pragma GCC pop_options
#endif //SIMD_LINEAR_BASE_64_H
