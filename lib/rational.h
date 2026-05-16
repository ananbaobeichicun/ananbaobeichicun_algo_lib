#ifndef UNTITLED_RATIONAL_H
#define UNTITLED_RATIONAL_H

#include <exception>
#include <iostream>

template<typename T>
struct rational {
    T n, m;

    constexpr rational &reduce() {
        if (!m)
            throw std::exception{};
        const T d = gcd(m, n);
        n /= d;
        m /= d;
        if (m < 0) {
            n = -n;
            m = -m;
        }
        return *this;
    }

    constexpr rational(T n = 0, T m = 1) : n(n), m(m) {
        reduce();
    }

    [[nodiscard]] constexpr operator bool() const {
        return n;
    }

    [[nodiscard]] constexpr rational operator+() const {
        return *this;
    }

    [[nodiscard]] constexpr rational operator-() const {
        return {-n, m};
    }

    [[nodiscard]] constexpr rational inv() const {
        if (!*this)
            throw std::exception{};
        return {m, n};
    }
};

template<typename T>
constexpr bool operator==(const rational<T> x, const rational<T> y) {
    return x.n == y.n && x.m == y.m;
}

template<typename T>
constexpr bool operator<(const rational<T> x, const rational<T> y) {
    return x.n * y.m < y.n * x.m;
}

template<typename T>
constexpr std::strong_ordering operator<=>(const rational<T> x, const rational<T> y) {
    if (x == y)
        return std::strong_ordering::equal;
    if (x < y)
        return std::strong_ordering::less;
    else
        return std::strong_ordering::greater;
}

template<typename T>
constexpr rational<T> operator+(const rational<T> x, const rational<T> y) {
    return rational<T>{x.n * y.m + y.n * x.m, x.m * y.m}.reduce();
}

template<typename T>
constexpr rational<T> operator-(const rational<T> x, const rational<T> y) {
    return x + (-y);
}

template<typename T>
constexpr rational<T> operator*(const rational<T> x, const rational<T> y) {
    return rational<T>{x.n * y.n, x.m * y.m}.reduce();
}

template<typename T>
constexpr rational<T> operator/(const rational<T> x, const rational<T> y) {
    return x * y.inv();
}

#define DEFINE_ASSIGNMENT_OPERATOR(op) \
template<typename T> \
constexpr rational<T> &operator op##=(rational<T> &x, const rational<T> y) { \
    return x = x op y; \
}

DEFINE_ASSIGNMENT_OPERATOR(+)
DEFINE_ASSIGNMENT_OPERATOR(-)
DEFINE_ASSIGNMENT_OPERATOR(*)
DEFINE_ASSIGNMENT_OPERATOR(/)

template<typename T>
std::istream &operator>>(std::istream &is, rational<T> &x) {
    std::string buffer;
    is >> buffer;
    const size_t pos = buffer.find('/');
    if (pos == std::string::npos)
        x = rational<T>{std::stoll(buffer)};
    else
        x = rational<T>{std::stoll(buffer.substr(0, pos)), std::stoll(buffer.substr(pos + 1))};
    return is;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const rational<T> &x) {
    os << x.n;
    if (x.m != 1)
        os << "/" << x.m;
    return os;
}

#endif //UNTITLED_RATIONAL_H