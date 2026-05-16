#ifndef EULER_SIEVE_H
#define EULER_SIEVE_H
#include <array>
#include <vector>
#include <variant>

template<typename T, size_t N, bool enable_phi = false, bool enable_mu = false>
struct euler_sieve {
    std::array<T, N + 1> least_factor;
    std::vector<T> primes;
    [[no_unique_address]] std::conditional_t<enable_phi, std::array<T, N + 1>, std::monostate> phi;
    [[no_unique_address]] std::conditional_t<enable_mu, std::array<T, N + 1>, std::monostate> mu;

    constexpr explicit euler_sieve() {
        if constexpr (enable_phi)
            phi[1] = 1;
        if constexpr (enable_mu)
            mu[1] = 1;
        for (T i = 2; i <= N; ++i) {
            if (!least_factor[i]) {
                least_factor[i] = i;
                if constexpr (enable_phi)
                    phi[i] = i - 1;
                if constexpr (enable_mu)
                    mu[i] = -1;
                primes.push_back(i);
            }
            for (const T p: primes) {
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

#endif //EULER_SIEVE_H
