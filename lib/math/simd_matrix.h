#ifndef SIMD_MATRIX_H
#define SIMD_MATRIX_H
#include <cassert>
#include <cstdint>
#include <immintrin.h>
#include <new>
#include <algorithm>
#include "../number/montgomery_32.h"

#pragma GCC push_options
#pragma GCC target("sse3", "sse2", "sse", "sse4", "sse4.1", "sse4.2", "ssse3", "avx2", "avx512f", "avx512dq", "avx512vl")

// NOLINTBEGIN(*-simd-intrinsics)
// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
    namespace simd_matrix_impl {
        void add(const uint64_t *__restrict const src, uint64_t *__restrict const dst, const size_t n) {
            size_t i;
#pragma GCC unroll(4)
            for (i = 0; i + 8 <= n; i += 8) {
                const __m512i src_chunk = _mm512_loadu_si512(src + i);
                __m512i dst_chunk = _mm512_loadu_si512(dst + i);
                dst_chunk = _mm512_add_epi64(dst_chunk, src_chunk);
                _mm512_storeu_si512(dst + i, dst_chunk);
            }
            for (; i < n; ++i)
                dst[i] += src[i];
        }

        void sub(const uint64_t *__restrict const src, uint64_t *__restrict const dst, const size_t n) {
            size_t i;
#pragma GCC unroll(4)
            for (i = 0; i + 8 <= n; i += 8) {
                const __m512i src_chunk = _mm512_loadu_si512(src + i);
                __m512i dst_chunk = _mm512_loadu_si512(dst + i);
                dst_chunk = _mm512_sub_epi64(dst_chunk, src_chunk);
                _mm512_storeu_si512(dst + i, dst_chunk);
            }
            for (; i < n; ++i)
                dst[i] += src[i];
        }

        void mul(const uint64_t *const M, const uint64_t *const N, uint64_t *__restrict const dst,
                 const size_t m, const size_t k, const size_t n) {
            for (size_t i = 0; i < m; ++i) {
                for (size_t j = 0; j < k; ++j) {
                    const __m512i M_chunk = _mm512_set1_epi64((int64_t) M[i * k + j]);

                    size_t l;
#pragma GCC unroll(4)
                    for (l = 0; l + 8 <= n; l += 8) {
                        __m512i N_chunk = _mm512_loadu_si512(N + j * n + l);
                        N_chunk = _mm512_mul_epu32(M_chunk, N_chunk);
                        __m512i res_chunk = _mm512_loadu_si512(dst + i * n + l);
                        res_chunk = _mm512_add_epi64(res_chunk, N_chunk);
                        _mm512_storeu_si512(dst + i * n + l, res_chunk);
                    }
                    for (; l < n; ++l)
                        dst[i * n + l] += M[i * k + j] * N[j * n + l];
                }
            }
        }

        /**
         * calculate product of two matrix within *montgomery* field (by simd)
         * the requirement that `k < 1uz << 31` is to guarantee `res_chunk` not to overflow
         * when return, `result` is guaranteed to equiv to real ans but left unmoded
         *
         * @param M pointer pointing to first element of left operator
         * @param N pointer pointing to first element of right operator
         * @param dst pointer pointing to first element of result
         * @param m height of matrix M
         * @param k width of matrix M aka height of matrix N
         * @param n width of matrix N
         * @param mont montgomery object
         */
        void mul_montgomery_raw(const uint64_t *const M, const uint64_t *const N, uint64_t *__restrict const dst,
                                const size_t m, const size_t k, const size_t n, const montgomery_32 mont) {
            assert(k < 1uz << 31);
            for (size_t i = 0; i < m; ++i) {
                size_t l;
                for (l = 0; l + 8 <= n; l += 8) {
                    __m512i res_chunk = _mm512_setzero_si512();
#pragma GCC unroll(4)
                    for (size_t j = 0; j + 2 <= k; j += 2) {
                        const __m512i M_chunk0 = _mm512_set1_epi64((int64_t) M[i * k + j]);
                        const __m512i N_chunk0 = _mm512_loadu_si512(N + j * n + l);
                        const __m512i M_chunk1 = _mm512_set1_epi64((int64_t) M[i * k + (j + 1)]);
                        const __m512i N_chunk1 = _mm512_loadu_si512(N + (j + 1) * n + l);
                        res_chunk = _mm512_add_epi64(res_chunk,
                                                     mont._mm512_raw_REDC(_mm512_add_epi64(
                                                             _mm512_mul_epu32(M_chunk0, N_chunk0),
                                                             _mm512_mul_epu32(M_chunk1, N_chunk1)
                                                         )
                                                     ));
                    }
                    if (k & 1) {
                        const __m512i M_chunk = _mm512_set1_epi64((int64_t) M[i * k + (k - 1)]);
                        const __m512i N_chunk = _mm512_loadu_si512(N + (k - 1) * n + l);
                        res_chunk = _mm512_add_epi64(res_chunk, mont._mm512_raw_mul(M_chunk, N_chunk));
                    }

                    _mm512_storeu_si512(dst + i * n + l, res_chunk);
                }
                for (size_t ll = l; ll < n; ++ll) {
                    dst[i * n + ll] = 0;
                    for (size_t j = 0; j < k; ++j)
                        dst[i * n + ll] += mont.mul((uint32_t) M[i * k + j], (uint32_t) N[j * n + ll]);
                }
            }
        }

        /**
         * basically same as `mul_montgomery_raw` except for the result is properly modded
         */
        void mul_montgomery(const uint64_t *const M, const uint64_t *const N, uint64_t *__restrict const dst,
                            const size_t m, const size_t k, const size_t n, const montgomery_32 mont) {
            mul_montgomery_raw(M, N, dst, m, k, n, mont);
            mont.batch_REDC(dst, dst, m * n);
            mont.batch_encode(dst, dst, m * n);
        }

        /**
         * multiply a matrix by power of a matrix within *montgomery* field (by simd)
         * i.e. N = M^t * N
         * @warning `src` need to be align to 64 bytes boundary
         * @warning `src` will be modified
         *
         * @param src base matrix M
         * @param dst the matrix N, also being the place to store result
         * @param n order of `src`
         * @param t exponent
         * @param mont montgomery object
         */
        void mod_pow_montgomery(uint64_t *__restrict const src, uint64_t *__restrict const dst,
                                const size_t n, size_t t, const montgomery_32 mont) {
            assert(!((uintptr_t) src % 64));
            const auto acc = (uint64_t *) operator new(sizeof(uint64_t) * n * n, std::align_val_t(64));
            const auto tmp = (uint64_t *) operator new(sizeof(uint64_t) * n * n, std::align_val_t(64));
            bool flag = false;
            while (t) {
                if (t & 1) {
                    if (flag)
                        mul_montgomery(src, tmp, dst, n, n, n, mont);
                    else
                        mul_montgomery(src, dst, tmp, n, n, n, mont);
                    flag = !flag;
                }
                mul_montgomery(src, src, acc, n, n, n, mont);
                t >>= 1;

                if (!t)
                    break;
                if (t & 1) {
                    if (flag)
                        mul_montgomery(acc, tmp, dst, n, n, n, mont);
                    else
                        mul_montgomery(acc, dst, tmp, n, n, n, mont);
                    flag = !flag;
                }
                mul_montgomery(acc, acc, src, n, n, n, mont);
                t >>= 1;
            }

            if (flag)
                std::ranges::copy_n(tmp, ptrdiff_t(n * n), dst);
            operator delete(acc, std::align_val_t(64));
            operator delete(tmp, std::align_val_t(64));
        }
    }
}

// NOLINTEND(*-simd-intrinsics)
#pragma GCC pop_options

#endif //SIMD_MATRIX_H
