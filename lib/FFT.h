#ifndef FFT_H
#define FFT_H
#include <complex>
#include <vector>

constexpr std::vector<size_t> get_rev(const size_t n) {
    std::vector<size_t> ans(1 << n);
    for (size_t i = 0; i < 1 << n; ++i)
        ans[i] = ans[i >> 1] >> 1 | (i & 1) << n - 1;
    return ans;
}

void FFT(std::vector<std::complex<double> > &vec, const bool is_normal) {
    const size_t n = vec.size();
    const std::vector<size_t> rev = get_rev(std::__lg(n));
    for (size_t i = 0; i < n; ++i)
        if (i < rev[i])
            swap(vec[i], vec[rev[i]]);

    constexpr double pi = std::numbers::pi;

    for (size_t i = 2; i <= n; i <<= 1) {
        // NOLINT(*-narrowing-conversions)
        const std::complex<double> w1 = is_normal
                                            ? std::complex(cos(2 * pi / i), sin(2 * pi / i))
                                            : std::complex(cos(2 * pi / i), -sin(2 * pi / i));
        for (auto j = vec.begin(); j != vec.end(); j += i) {
            std::complex<double> wk(1, 0);
            for (auto k = j; k != j + (i >> 1); ++k) {
                const std::complex<double> x = *k, y = *(k + (i >> 1));
                *k = x + y * wk;
                *(k + (i >> 1)) = x - y * wk;
                wk *= w1;
            }
        }
    }

    if (!is_normal)
        for (auto &i: vec)
            i /= n;
}

#endif //FFT_H
