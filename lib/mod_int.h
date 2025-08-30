#ifndef MOD_INT_H
#define MOD_INT_H

#include <concepts>
#include <iostream>
#include <sstream>
#include <format>
#include <cassert>

template<std::unsigned_integral T, T MOD>
struct mod_int {
    T data;

    constexpr mod_int() = default;

    constexpr mod_int(T data) : data(data % MOD) {
    }

    [[nodiscard]] constexpr mod_int inv() const {
        if (data == 0)
            throw std::exception{};
        return pow(*this, MOD - 2);
    }

    std::pair<T, T> guess_ratio(T n) const {
        assert(n <= MOD);
        std::pair<T, T> ans{MOD - 1, MOD - 1};
        for (T i = 1; i < n; ++i) {
            const std::pair<T, T> cur{i * data % MOD, i};
            if (ans.first * ans.first + ans.second * ans.second > cur.first * cur.first + cur.second * cur.second)
                ans = cur;
        }
        return ans;
    }

    static bool unchecked_lt(const mod_int x, const mod_int y) {
        return x.data < y.data;
    }

    static bool unchecked_gt(const mod_int x, const mod_int y) {
        return x.data > y.data;
    }

    static bool unchecked_le(const mod_int x, const mod_int y) {
        return x.data <= y.data;
    }

    static bool unchecked_ge(const mod_int x, const mod_int y) {
        return x.data >= y.data;
    }

    constexpr explicit operator bool() const {
        return data;
    }

    constexpr explicit operator T() const {
        return data;
    }
};

template<std::unsigned_integral T, T MOD>
std::ostream &operator<<(std::ostream &os, const mod_int<T, MOD> &x) {
    return os << x.data;
}

template<std::unsigned_integral T, T MOD>
std::istream &operator>>(std::istream &is, mod_int<T, MOD> &x) {
    return is >> x.data;
}

template<std::unsigned_integral T, T MOD>
struct std::formatter<mod_int<T, MOD>, char> {
    template<class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext &ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}')
            throw std::format_error{""};
        return it;
    }

    template<class FmtContext>
    static typename FmtContext::iterator format(mod_int<T, MOD> s, FmtContext &ctx) {
        std::ostringstream out;
        out << s;
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> &operator++(mod_int<T, MOD> &x) {
    if (++x.data == MOD)
        [[unlikely]] x.data = 0;
    return x;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator++(mod_int<T, MOD> &x, int) {
    mod_int<T, MOD> ret = x;
    if (++x.data == MOD)
        [[unlikely]] x.data = 0;
    return ret;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> &operator--(mod_int<T, MOD> &x) {
    if (!x.data--)
        x.data = MOD - 1;
    return x;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator--(mod_int<T, MOD> &x, int) {
    mod_int<T, MOD> ret = x;
    if (!x.data--)
        x.data = MOD - 1;
    return ret;
}

template<std::unsigned_integral T, T MOD>
constexpr bool operator==(const mod_int<T, MOD> x, const mod_int<T, MOD> y) {
    return x.data == y.data;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator+(mod_int<T, MOD> x) {
    return x;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator-(mod_int<T, MOD> x) {
    return x.data ? MOD - x.data : x.data;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator+(const mod_int<T, MOD> x, const mod_int<T, MOD> y) {
    if (x.data + y.data >= MOD)
        return x.data + y.data - MOD;
    return x.data + y.data;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator-(const mod_int<T, MOD> x, const mod_int<T, MOD> y) {
    if (x.data >= y.data)
        return x.data - y.data;
    return MOD - y.data + x.data;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator*(const mod_int<T, MOD> x, const mod_int<T, MOD> y) {
    return x.data * y.data % MOD;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> operator/(const mod_int<T, MOD> x, const mod_int<T, MOD> y) {
    return x * y.inv();
}

#define DEFINE_MIXED_OPERATOR(op) \
template<std::unsigned_integral T, T MOD> \
constexpr mod_int<T, MOD> operator op (const T x, const mod_int<T, MOD> y) { \
    return mod_int<T, MOD>{x} op y; \
} \
 \
template<std::unsigned_integral T, T MOD> \
constexpr mod_int<T, MOD> operator op (const mod_int<T, MOD> x, const T y) { \
    return x op mod_int<T, MOD>{y}; \
}

DEFINE_MIXED_OPERATOR(+)
DEFINE_MIXED_OPERATOR(-)
DEFINE_MIXED_OPERATOR(*)
DEFINE_MIXED_OPERATOR(/)

#define DEFINE_ASSIGNMENT_OPERATOR(op, assignment_op) \
template<std::unsigned_integral T, T MOD> \
constexpr mod_int<T, MOD> &operator assignment_op(mod_int<T, MOD> &x, const mod_int<T, MOD> y) { \
    return x = x op y; \
} \
 \
template<std::unsigned_integral T, T MOD> \
constexpr mod_int<T, MOD> &operator assignment_op(mod_int<T, MOD> &x, const T y) { \
    return x assignment_op mod_int<T, MOD>{y}; \
}
DEFINE_ASSIGNMENT_OPERATOR(+, +=)
DEFINE_ASSIGNMENT_OPERATOR(-, -=)
DEFINE_ASSIGNMENT_OPERATOR(*, *=)
DEFINE_ASSIGNMENT_OPERATOR(/, /=)

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> pow(mod_int<T, MOD> x, T y) {
    mod_int<T, MOD> ans = 1;
    while (y) {
        if (y & 1)
            ans *= x;
        x *= x;
        y >>= 1;
    }
    return ans;
}

template<std::unsigned_integral T, T MOD>
constexpr mod_int<T, MOD> naive_fact(mod_int<T, MOD> n) {
    mod_int<T, MOD> ans = 1;
    for (mod_int<T, MOD> i = 1; i <= n; ++i)
        ans *= i;
    return ans;
}

#define def_mod_type(T, MOD_VAL) constexpr T MOD = (MOD_VAL); \
using m##T = mod_int<T, MOD>; \
constexpr m##T operator""_m##T(const unsigned long long x) { \
    return m##T(x); \
}

#endif //MOD_INT_H
