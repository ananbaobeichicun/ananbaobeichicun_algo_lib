#ifndef MONTGOMERY_32_H
#define MONTGOMERY_32_H
#include <cstdint>
#include <cassert>

[[gnu::always_inline]] constexpr static uint32_t inv_mod_2_32(const uint32_t x) {
    uint32_t y = 1;
    for (int i = 0; i < 5; ++i)
        y *= 2 - x * y;
    return y;
}

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
    struct montgomery_32 {
        constexpr static uint64_t r = 1ull << 32;
        const uint32_t n, nn, r2;

        constexpr explicit montgomery_32(const uint32_t n) : n(n), nn(-inv_mod_2_32(n)),
                                                             r2(uint32_t(-uint64_t(n) % uint64_t(n))) {
            assert(n & 1 && !(n & 1u << 31));
        }

        [[nodiscard]] constexpr uint32_t justify(const uint32_t x) const {
            return x >= n ? x - n : x;
        }

        [[nodiscard]] constexpr uint32_t raw_REDC(const uint64_t x) const {
            const uint32_t m = (uint32_t)x * nn;
            return uint32_t((x + (uint64_t) m * n) >> 32);
        }

        [[nodiscard]] constexpr uint32_t REDC(const uint64_t x) const {
            return justify(raw_REDC(x));
        }

        [[nodiscard]] constexpr uint32_t raw_mul(const uint32_t x, const uint32_t y) const {
            return raw_REDC((uint64_t)x * y);
        }

        [[nodiscard]] constexpr uint32_t mul(const uint32_t x, const uint32_t y) const {
            return REDC((uint64_t)x * y);
        }

        [[nodiscard]] constexpr uint32_t encode(const uint32_t x) const {
            return mul(x, r2);
        }

        [[nodiscard]] uint32_t constexpr add(const uint32_t x, uint32_t y) const {
            return justify(x + y);
        }

        [[nodiscard]] uint32_t constexpr sub(const uint32_t x, uint32_t y) const {
            return justify(x + n - y);
        }
    };
}

#endif //MONTGOMERY_32_H
