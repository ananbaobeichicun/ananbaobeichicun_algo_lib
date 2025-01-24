#ifndef PRIME_H
#define PRIME_H

#include <utility>
#include <vector>
#include <map>
#include <bitset>
#include <ranges>

using ull = unsigned long long;

constexpr bool is_prime(const ull n) {
    if (n < 2)
        return false;
    for (ull i = 2; i * i <= n; ++i)
        if (!(n % i))
            return false;
    return true;
}

template<size_t n>
constexpr std::bitset<n> prime_filter() {
    std::bitset<n> is_not_prime;
    set::vector<n> primes.resize(n / log(n));
    for (ull i = 2; i < n; ++i){
        if(!is_not_prime[i])
            primes.push_back[i];

    }
}

constexpr std::vector<ull> get_prime_vec(const ull n) {
    std::vector<ull> ans;
    for (ull i = 2; i <= n; ++i)
        if (is_prime(i))
            ans.push_back(i);
    return ans;
}

//experimental
template<size_t maxn>
constexpr std::vector<size_t> get_prime_vec(const size_t n) {
    std::vector<ull> ans;
    auto filter = prime_filter<maxn>();
    for (ull i = 2; i <= n; ++i)
        if(filter[i])
            ans.push_back(i);
    return ans;
}

template<size_t maxn>
std::vector<ull> factor_int(ull n) {
    if (!n)
        throw std::invalid_argument("n == 0");
    std::vector<ull> ans;
    for (ull i: get_prime_vec<maxn>(n)) {
        if (n == 1)
            return ans;
        while (!(n % i)) {
            n /= i;
            ans.push_back(i);
        }
    }
    if(n == 1)
        return ans;
    std::unreachable();
}

//fixme experimental
std::vector<ull> factor_int(ull n) {
    if (!n)
        throw std::invalid_argument("n == 0");
    std::vector<ull> ans;
    for (ull i: get_prime_vec(n)) {
        if (n == 1)
            return ans;
        while (!(n % i)) {
            n /= i;
            ans.push_back(i);
        }
    }
    if(n == 1)
        return ans;
    std::unreachable();
}

ull prime_pi(const ull n) {
    return factor_int(n).size();
}

#endif //PRIME_H
