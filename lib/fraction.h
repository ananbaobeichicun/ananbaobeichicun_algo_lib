#ifndef FRACTION_H
#define FRACTION_H
#include <iostream>
#include <numeric>
#include <cassert>

template<std::integral T> //todo 加强约束为带符号数
class fraction {
public:
    T numerator, denominrator;

    explicit fraction(T numerator = 0, T denominrator = 1, const bool require_reduce = true): numerator(numerator),
        denominrator(denominrator) {
        if (require_reduce)
            reduce();
    }

    fraction &reduce() {
        // assert(denominrator);
        T res = std::gcd(numerator, denominrator);
        numerator /= res;
        denominrator /= res;
        if (denominrator < 0) {
            numerator *= -1;
            denominrator *= -1;
        }
        return *this;
    }

    // NOLINTNEXTLINE(*-explicit-constructor)
    constexpr operator double() const {
        return static_cast<double>(numerator) / denominrator;
    }

    constexpr explicit operator bool() const {
        return numerator;
    }
};

template<typename T>
std::istream &operator>>(std::istream &in, fraction<T> &f) {
    std::string buffer;
    in >> buffer;
    if (size_t pos; (pos = buffer.find('/')) != std::string::npos) {
        f.numerator = stoll(buffer.substr(0, pos - 1));
        f.denominrator = stoll(buffer.substr(pos));
    } else {
        f.numerator = stoll(buffer);
        f.denominrator = 1;
    }
    return in;
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const fraction<T> f) {
    if (f.numerator == 0 || f.denominrator == 1) {
        return out << f.numerator;
    }
    return out << f.numerator << "/" << f.denominrator;
}

template<typename T>
constexpr bool operator==(const fraction<T> x, const fraction<T> y) {
    return x.numerator == y.numerator && x.denominrator == y.denominrator;
}

template<typename T>
constexpr std::strong_ordering operator<=>(const fraction<T> x, const fraction<T> y) {
    if (x == y)
        return std::strong_ordering::equal;
    if (x.numerator < 0 && y.numerator > 0)
        return std::strong_ordering::less;
    if (x.numerator > 0 && y.numerator < 0)
        return std::strong_ordering::greater;
    fraction<T> res = x / y;
    bool ans = res.numerator < res.denominrator;
    return (ans != (x.numerator < 0)) ? std::strong_ordering::less : std::strong_ordering::greater;
}

template<typename T>
constexpr fraction<T> operator+(const fraction<T> x) {
    return x;
}

template<typename T>
constexpr fraction<T> operator-(const fraction<T> x) {
    return fraction<T>{-x.numerator, x.denominrator};
}

template<typename T>
constexpr fraction<T> operator+(const fraction<T> x, const fraction<T> y) {
    const T res = std::lcm(x.denominrator, y.denominrator);
    const T x_scale = res / x.denominrator;
    const T y_scale = res / y.denominrator;
    return fraction<T>{x.numerator * x_scale + y.numerator * y_scale, res};
}

template<typename T>
constexpr fraction<T> operator-(const fraction<T> x, const fraction<T> y) {
    return x + (-y);
}

template<typename T>
constexpr fraction<T> operator*(const fraction<T> x, const fraction<T> y) {
    fraction<T> a = fraction<T>{x.numerator, y.denominrator};
    fraction<T> b = fraction<T>{y.numerator, x.denominrator};
    return fraction<T>{a.numerator * b.numerator, a.denominrator * b.denominrator, false};
}

template<typename T>
constexpr fraction<T> operator/(const fraction<T> x, const fraction<T> y) {
    return x * fraction<T>{y.denominrator, y.numerator};
}


template<typename T>
fraction<T> &operator+=(fraction<T> &x, const fraction<T> y) {
    return x = x + y;
}

template<typename T>
fraction<T> &operator-=(fraction<T> &x, const fraction<T> y) {
    return x = x - y;
}

template<typename T>
fraction<T> &operator*=(fraction<T> &x, const fraction<T> y) {
    return x = x * y;
}

template<typename T>
fraction<T> &operator/=(fraction<T> &x, const fraction<T> y) {
    return x = x / y;
}

#endif //FRACTION_H
