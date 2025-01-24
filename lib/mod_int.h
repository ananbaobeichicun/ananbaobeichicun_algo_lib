#ifndef MOD_INT_H
#define MOD_INT_H

#include <concepts>
#include <iostream>

template<std::integral T, T MOD>
struct mod_int {
    T data;

    constexpr mod_int() = default;

    constexpr mod_int(T data) : data(data % MOD) {
    }

    [[nodiscard]] constexpr mod_int inv() const {
        if (data == 0)
            throw std::exception();
        return fast_pow(*this, MOD - 2);
    }
};

template<std::integral T, T MOD>
std::ostream &operator<<(std::ostream &os, const mod_int<T, MOD> &x) {
    return os << x.data;
}

template<std::integral T, T MOD>
std::istream &operator>>(std::istream &is, mod_int<T, MOD> &x) {
    return is >> x.data;
}

template<std::integral T, T MOD>
std::strong_ordering operator<=>(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    return x.data <=> y.data;
}

template<std::integral T, T MOD>
std::strong_ordering operator<=>(T x, mod_int<T, MOD> y) {
    return x <=> y.data;
}

template<std::integral T, T MOD>
std::strong_ordering operator<=>(mod_int<T, MOD> x, T y) {
    return x.data <=> y;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator+(mod_int<T, MOD> x) {
    return x;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator-(mod_int<T, MOD> x) {
    return x.data ? MOD - x.data : x.data;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator+(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    if (x.data + y.data >= MOD)
        return x.data + y.data - MOD;
    return x.data + y.data;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator+(T x, mod_int<T, MOD> y) {
    return mod_int<T, MOD>{x} + y;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator+(mod_int<T, MOD> x, T y) {
    return x + mod_int<T, MOD>{y};
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator-(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    if (x.data >= y.data)
        return x.data - y.data;
    return MOD - y.data + x.data;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator-(T x, mod_int<T, MOD> y) {
    return mod_int<T, MOD>{x} - y;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator-(mod_int<T, MOD> x, T y) {
    return x - mod_int<T, MOD>{y};
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator*(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    return x.data * y.data % MOD;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator*(T x, mod_int<T, MOD> y) {
    return mod_int<T, MOD>{x} * y;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator*(mod_int<T, MOD> x, T y) {
    return x * mod_int<T, MOD>{y};
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator/(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    return x * y.inv();
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator/(T x, mod_int<T, MOD> y) {
    return mod_int<T, MOD>{x} / y;
}

template<std::integral T, T MOD>
mod_int<T, MOD> operator/(mod_int<T, MOD> x, T y) {
    return x / mod_int<T, MOD>{y};
}


template<std::integral T, T MOD>
constexpr mod_int<T, MOD> fast_pow(mod_int<T, MOD> x, T y) {
    mod_int<T, MOD> ans = 1;
    while (y) {
        if (y & 1)
            ans *= x;
        x *= x;
        y >>= 1;
    }
    return ans;
}

template<std::integral T, T MOD>
constexpr mod_int<T, MOD> fast_pow(mod_int<T, MOD> x, mod_int<T, MOD> y) {
    return fast_pow(x, y.data);
}

template<std::integral T, T MOD>
mod_int<T, MOD>& operator+=(mod_int<T, MOD> &x, mod_int<T, MOD> y) {
    return x = x + y;
}

template<std::integral T, T MOD>
mod_int<T, MOD>& operator+=(mod_int<T, MOD> &x, T y) {
    return x = x + mod_int<T, MOD>{y};
}

template<std::integral T, T MOD>
mod_int<T, MOD>& operator-=(mod_int<T, MOD> &x, mod_int<T, MOD> y) {
    return x = x - y;
}

template<std::integral T, T MOD>
mod_int<T, MOD>& operator*=(mod_int<T, MOD> &x, mod_int<T, MOD> y) {
    return x = x * y;
}

template<std::integral T, T MOD>
mod_int<T, MOD>& operator/=(mod_int<T, MOD> &x, mod_int<T, MOD> y) {
    return x = x / y;
}
#endif //MOD_INT_H
