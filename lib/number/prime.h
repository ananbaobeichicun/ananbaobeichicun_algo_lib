#ifndef PRIME_H
#define PRIME_H
#include <utility>
#include <vector>
#include <ranges>

using ull = unsigned long long;

template<std::unsigned_integral T>
constexpr bool is_prime_naive(const T n) {
    if (n < 2)
        return false;
    for (T i = 2; i * i <= n; ++i)
        if (n % i == 0)
            return false;
    return true;
}

template<std::unsigned_integral T, std::output_iterator<T> It>
void get_factors(T n, It it) {
    if (!n)
        throw std::invalid_argument{""};

    for (T i = 2; i * i <= n; ++i) {
        while (n % i == 0) {
            *it++ = i;
            n /= i;
        }
    }

    if (n != 1)
        *it++ = n;
}

struct empty_dummy {
};

using ll = long long;
using ull = unsigned long long;

template<size_t N, bool enable_phi = false, bool enable_mu = false>
struct euler_sieve {
    std::array<ull, N + 1> least_factor;
    std::vector<ull> primes;
    [[no_unique_address]] std::conditional_t<enable_phi, std::array<ull, N + 1>, empty_dummy> phi;
    [[no_unique_address]] std::conditional_t<enable_mu, std::array<ll, N + 1>, empty_dummy> mu;

    constexpr explicit euler_sieve() {
        if constexpr (enable_phi)
            phi[1] = 1;
        if constexpr (enable_mu)
            mu[1] = 1;
        for (ull i = 2; i <= N; ++i) {
            if (!least_factor[i]) {
                least_factor[i] = i;
                if constexpr (enable_phi)
                    phi[i] = i - 1;
                if constexpr (enable_mu)
                    mu[i] = -1;
                primes.push_back(i);
            }
            for (const ull p: primes) {
                if (p * i > N)
                    break;
                least_factor[p * i] = p;
                if (!(i % p)) {
                    if constexpr (enable_phi)
                        phi[p * i] = phi[i] * p;
                    if constexpr (enable_mu)
                        mu[p * i] = 0;
                    break;
                }
                if constexpr (enable_phi)
                    phi[p * i] = phi[i] * (p - 1);
                if constexpr (enable_mu)
                    mu[p * i] = -mu[i];
            }
        }
    }
};
#endif //PRIME_H
