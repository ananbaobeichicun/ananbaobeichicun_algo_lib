#ifndef STR_HASH_H
#define STR_HASH_H

#include "mod_int.h"
#define ull unsigned long long

// 默认小端序
template<ull BASE, ull MOD>
class str_hash {
    using mod_t = mod_int<ull, MOD>;
    std::string str;
    mod_t val;

public:
    [[nodiscard]] constexpr const std::string &get_str() const { return str; };
    [[nodiscard]] constexpr ull get_val() const { return val.data; }

    constexpr explicit str_hash(std::string s): str(std::move(s)) {
        const char *const raw = str.data();
        const char *const end_ptr = raw + str.size();
        const char *const aligned_end = raw + str.size() / 4 * 4;

        val = 0;
        for (const char *p = end_ptr - 1; p >= aligned_end; --p)
            (val.data <<= 8) |= *p;

        for (const char *p = aligned_end - 4; p >= raw; p -= 4)
            (val *= BASE) += static_cast<ull>(*reinterpret_cast<const unsigned *>(p));
    }

    constexpr str_hash &modify(const size_t idx, const char c) {
        const size_t dword_bias = idx % 4 * 8;
        const size_t dword_idx = idx / 4;

        mod_t delta = MOD + c - str[idx];
        delta *= 1ull << dword_bias;
        delta *= pow(mod_t{BASE}, dword_idx);

        val += delta;

        str[idx] = c;

        return *this;
    }

    // constexpr str_hash &push_back(const char c) {
    //
    // }
};

#undef ull

#endif //STR_HASH_H
