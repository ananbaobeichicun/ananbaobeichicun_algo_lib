// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
#ifndef BIG_INT_H
#define BIG_INT_H

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

using ull = unsigned long long;

class u_big_int {
public:
    enum class reserve_t { reserve };

    constexpr static ull limit = 1e9, limit_digit = 9;
    constexpr static auto reserve = reserve_t::reserve;
    std::vector<ull> data; //小端序

    [[nodiscard]] size_t len() const {
        return data.size();
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    u_big_int(const ull v = 0): data({v}) {
    }

    u_big_int(const size_t s, reserve_t) {
        data.reserve(s);
    }

    u_big_int(const std::initializer_list<ull> &v): data(v) {
    }

    explicit u_big_int(std::string_view str) {
        for (auto i = str.rbegin(); i < str.rend(); i += limit_digit) {
            std::string slice(i, std::min(i + limit_digit, str.rend()));
            std::ranges::reverse(slice);
            data.push_back(std::stoull(slice));
        }
    }

    ull &operator[](const size_t index) {
        return data[index];
    }

    const ull &operator[](const size_t index) const {
        return data[index];
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    // NOLINTNEXTLINE(*-explicit-constructor)
    operator ull() const {
        return data[0];
    }

    u_big_int &shrink() {
        while (!data.back())
            data.pop_back();
        return *this;
    }
};

constexpr size_t min_len(const u_big_int &x, const u_big_int &y) {
    return std::min(x.len(), y.len());
}

constexpr size_t max_len(const u_big_int &x, const u_big_int &y) {
    return std::max(x.len(), y.len());
}

std::istream &operator>>(std::istream &input, u_big_int &n) {
    std::string tmp;
    input >> tmp;
    //除正号
    if (tmp[0] == '+')
        tmp = std::string(tmp.begin() + 1, tmp.end());
    n = u_big_int(tmp); //todo stringview和const string&的等同性
    return input;
}

std::ostream &operator<<(std::ostream &output, const u_big_int &n) {
    if (n.data.empty())
        output << '0';
    output << std::setfill('0');
    for (size_t i = n.len() - 1; ; --i) {
        if (i != n.len() - 1)
            output << std::setw(u_big_int::limit_digit);//补足位数
        output << n[i];
        if (!i)
            return output;
    }
}

std::weak_ordering operator<=>(const u_big_int &x, const u_big_int &y) {
    if (x.len() < y.len())
        return std::weak_ordering::less;
    if (x.len() > y.len())
        return std::weak_ordering::greater;
    for (size_t i = min_len(x, y) - 1; ; --i) {
        if (x.data[i] < y.data[i])
            return std::weak_ordering::less;
        if (x.data[i] > y.data[i])
            return std::weak_ordering::greater;
        if (i == 0)
            return std::weak_ordering::equivalent;
    }
}

u_big_int add_with_offset(const u_big_int &x, const u_big_int &y, size_t offset = 0, u_big_int ans) {
}

u_big_int operator+(u_big_int &&x, u_big_int &&y) {
    const auto &[m, M] = std::minmax(x, y);
    bool carry = false;
    u_big_int ans(M.len(), u_big_int::reserve);
    for (int i = 0; i < M.len(); ++i) {
        const ull px = i < m.len() ? m[i] : 0;
        const ull tmp_sum = px + M[i] + carry;
        ans.data.push_back(tmp_sum % u_big_int::limit);
        carry = tmp_sum / u_big_int::limit;
    }
    if (carry)
        ans.data.push_back(1);
    return ans;
}

u_big_int operator+(const u_big_int &x) {
    return x;
}

u_big_int operator+(const u_big_int &x, const u_big_int &y) {
    return u_big_int(x) + u_big_int(y);
}

// class s_big_int;
//     s_big_int::s_big_int(const ull, const bool) {
//     }
//
// s_big_int operator-(const u_big_int &x) {
//     return {x, false};
// }

u_big_int operator-(const u_big_int &x, const u_big_int &y) {
    assert(x >= y);
    bool carry = false;
    u_big_int ans(x.len(), u_big_int::reserve);
    for (int i = 0; i < x.len(); ++i) {
        const auto py = i < y.len() ? static_cast<long long>(y[i]) : 0;
        long long tmp_diff = static_cast<long long>(x[i]) - py - carry;
        if ((carry = tmp_diff < 0))
            tmp_diff += u_big_int::limit;
        ans.data.push_back(tmp_diff);
    }
    assert(!carry);
    return ans.shrink(); //todo 让它不复制
}

class s_big_int {
public:
    using reserve_t = u_big_int::reserve_t;
    constexpr static auto reserve = u_big_int::reserve;
    constexpr static ull limit = u_big_int::limit, limit_digit = u_big_int::limit_digit;

    u_big_int data;
    bool sign = true;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    s_big_int(u_big_int &&data, const bool sign = true): data(std::move(data)), sign(sign) {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    s_big_int(const u_big_int &data, const bool sign = true): data(data), sign(sign) {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    // NOLINTNEXTLINE(*-explicit-constructor)
    s_big_int(const ull v = 0, const bool sign = true): data({v}), sign(sign) {
    }

    s_big_int(const size_t s, reserve_t, const bool sign = true): data(s, u_big_int::reserve), sign(sign) {
    }

    // s_big_int(const std::initializer_list<ull> &v, const bool sign = true): data(v), sign(sign) {
    // }

    [[nodiscard]] constexpr long long get_sign() const {
        return sign ? 1 : -1;
    }

    operator long long() {
        return static_cast<long long>(data[0]) * get_sign();
    }
};

std::istream &operator>>(std::istream &input, s_big_int &n) {
    std::string tmp;
    input >> tmp;

    n.data.data.clear();
    n.sign = true;

    //除正号
    if (tmp[0] == '-') {
        n.sign = false;
        tmp = std::string(tmp.begin() + 1, tmp.end());
    }

    n.data = u_big_int(tmp);

    return input;
}

std::ostream &operator<<(std::ostream &output, const s_big_int &n) {
    if (!n.sign)
        output << '-';
    output << n.data;
    return output;
}

std::weak_ordering operator<=>(const s_big_int &x, const s_big_int &y) {
    if (x.sign) {
        if (y.sign)
            return x.data <=> y.data;
        return std::weak_ordering::greater;
    }
    if (y.sign)
        return std::weak_ordering::less;
    // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
    return 0 <=> (x.data <=> y.data);
}


s_big_int operator+(const s_big_int &x, const s_big_int &y) {
    if (x.sign == y.sign)
        return {x.data + y.data, x.sign};
    const auto &[m, M] = std::minmax(x, y);
    return {M.data - m.data, M.data == m.data ? true : M.sign};
}

s_big_int operator-(const s_big_int &x) {
    return {x.data, !x.sign};
}

s_big_int operator-(const s_big_int &x, const s_big_int &y) {
    return x + (-y);
}

#endif // BIG_INT_H
