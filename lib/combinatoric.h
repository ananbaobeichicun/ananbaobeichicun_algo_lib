#ifndef COMBINATORIC_H
#define COMBINATORIC_H
#include <concepts>
#include <vector>
#include <algorithm>

template<std::unsigned_integral T>
T fact(const T x) {
    T ans = 1;
    for (T i = 1; i <= x; ++i)
        ans *= i;
    return ans;
}

template<std::unsigned_integral T>
T binomial_coefficient(const T n, const T m) {
    if (m > n)
        return 0;
    T nominator = 1, denominator = 1;
    for (size_t i = n; i > n - m; --i)
        nominator *= i;
    for (size_t i = m; i; --i)
        denominator *= i;
    return nominator / denominator;
}

//fixme
// template<std::ranges::input_range R, std::integral T = typename R::value_type>
// T multinomial_coefficient(R r) {
//     T n = std::ranges::fold_left(r, 0, std::plus{});
//     T ans = 1;
//     for (T x: r)
//         for (T i = x; i; --i)
//             ans *= n-- / i;
//     return ans;
// }

#endif //COMBINATORIC_H
