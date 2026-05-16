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

template <std::unsigned_integral T, size_t N>
struct euler_sieve {
    std::array<T, N> least_factor{}, euler_phi{}, mu{};
    std::vector<T> primes;

    explicit euler_sieve (const T n) : least_factor(n + 1) {
        euler_phi[1] = 1;
        mu[1] = 1;
        for (T i = 2; i <= n; ++i) {
            if (!least_factor[i]) {
                least_factor[i] = i;
                euler_phi[i] = i - 1;
                mu[i] = -1;
                primes.push_back(i);
            }
            for (const T p: primes) {
                if (p * i > n)
                    break;
                least_factor[p * i] = p;
                if (!(i % p)) {
                    euler_phi[p * i] = euler_phi[i] * p;
                    mu[p * i] = 0;
                    break;
                }
                euler_phi[p * i] = euler_phi[i] * (p - 1);
                mu[p * i] = -mu[i];
            }
        }
    }
};
#endif //PRIME_H
