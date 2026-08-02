#ifndef MONTGOMERY_32_H
#define MONTGOMERY_32_H
#include <cassert>
#include <immintrin.h>
#pragma GCC push_options
#pragma GCC target("sse3", "sse2", "sse", "sse4", "sse4.1", "sse4.2", "ssse3", "avx2", "avx512f", "avx512dq", "avx512vl")

[[gnu::always_inline]] constexpr static uint32_t inv_mod_2_32(const uint32_t x) {
    uint32_t y = 1;
    for (int i = 0; i < 5; ++i)
        y *= 2 - x * y;
    return y;
}

// NOLINTBEGIN(*-simd-intrinsics)
// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
    struct montgomery_32 {
        constexpr static uint64_t r = 1ull << 32;
        const uint32_t n, nn, r2;

        constexpr static uint32_t check(const uint32_t n) {
            assert(n & 1 && !(n & 1u << 31));
            return n;
        }

        constexpr explicit montgomery_32(const uint32_t n) : n(check(n)), nn(-inv_mod_2_32(n)),
                                                             r2(uint32_t(-uint64_t(n) % uint64_t(n))) {
        }

        [[nodiscard]] constexpr uint32_t justify(const uint32_t x) const {
            return x >= n ? x - n : x;
        }

        [[nodiscard]] __m512i _mm512_justify(const __m512i chunk) const {
            const __m512i n_chunk = _mm512_set1_epi64(n);
            const __mmask8 mask = _mm512_cmpge_epu64_mask(chunk, n_chunk);
            return _mm512_mask_sub_epi64(chunk, mask, chunk, n_chunk);
        }

        [[nodiscard]] constexpr uint32_t raw_REDC(const uint64_t x) const {
            const uint32_t m = (uint32_t) x * nn;
            return uint32_t((x + (uint64_t) m * n) >> 32);
        }

        [[nodiscard]] __m512i _mm512_raw_REDC(__m512i chunk) const {
            const __m512i n_chunk = _mm512_set1_epi64(n);
            const __m512i nn_chunk = _mm512_set1_epi64(nn);
            __m512i tmp_chunk = _mm512_mul_epu32(chunk, nn_chunk);
            tmp_chunk = _mm512_mul_epu32(tmp_chunk, n_chunk);
            chunk = _mm512_add_epi64(chunk, tmp_chunk);
            return _mm512_srli_epi64(chunk, 32);
        }

        [[nodiscard]] constexpr uint32_t REDC(const uint64_t x) const {
            return justify(raw_REDC(x));
        }

        [[nodiscard]] __m512i _mm512_REDC(const __m512i chunk) const {
            return _mm512_justify(_mm512_raw_REDC(chunk));
        }

        void batch_REDC(const uint64_t *const src, uint64_t *const dst, const size_t n) const {
            size_t i;
#pragma GCC unroll(4)
            for (i = 0; i + 8 <= n; i += 8) {
                __m512i chunk = _mm512_loadu_si512(src + i);
                chunk = _mm512_REDC(chunk);
                _mm512_storeu_si512(dst + i, chunk);
            }
            for (; i < n; ++i)
                dst[i] = REDC(src[i]);
        }

        [[nodiscard]] constexpr uint32_t raw_mul(const uint32_t x, const uint32_t y) const {
            return raw_REDC((uint64_t) x * y);
        }

        [[nodiscard]] __m512i
        _mm512_raw_mul(const __m512i x_chunk, const __m512i y_chunk) const {
            return _mm512_raw_REDC(_mm512_mul_epu32(x_chunk, y_chunk));
        }

        [[nodiscard]] constexpr uint32_t mul(const uint32_t x, const uint32_t y) const {
            return REDC((uint64_t) x * y);
        }

        [[nodiscard]] __m512i _mm512_mul(const __m512i x_chunk, const __m512i y_chunk) const {
            return _mm512_REDC(_mm512_mul_epu32(x_chunk, y_chunk));
        }

        [[nodiscard]] constexpr uint32_t encode(const uint32_t x) const {
            return mul(x, r2);
        }

        [[nodiscard]] __m512i _mm512_encode(const __m512i chunk) const {
            const __m512i r2_chunk = _mm512_set1_epi64(r2);
            return _mm512_mul(chunk, r2_chunk);
        }

        void batch_encode(const uint64_t *const src, uint64_t *const dst, const size_t n) const {
            for (size_t i = 0; i < n; ++i)
                assert(src[i] < this->n);
            size_t i;
#pragma GCC unroll(4)
            for (i = 0; i + 8 <= n; i += 8) {
                __m512i chunk = _mm512_loadu_si512(src + i);
                chunk = _mm512_encode(chunk);
                _mm512_storeu_si512(dst + i, chunk);
            }
            for (; i < n; ++i)
                dst[i] = encode((uint32_t) src[i]);
        }

        [[nodiscard]] uint32_t constexpr add(const uint32_t x, const uint32_t y) const {
            return justify(x + y);
        }

        [[nodiscard]] __m512i _mm512_add(const __m512i x_chunk, const __m512i y_chunk) const {
            return _mm512_justify(_mm512_add_epi64(x_chunk, y_chunk));
        }

        [[nodiscard]] uint32_t constexpr sub(const uint32_t x, const uint32_t y) const {
            return justify(x + n - y);
        }

        [[nodiscard]] __m512i _mm512_sub(const __m512i x_chunk, const __m512i y_chunk) const {
            const __m512i n_chunk = _mm512_set1_epi64(n);
            return _mm512_justify(_mm512_sub_epi64(_mm512_add_epi64(x_chunk, n_chunk), y_chunk));
        }
    };
}

// NOLINTEND(*-simd-intrinsics)
#pragma GCC pop_options

#endif //MONTGOMERY_32_H
