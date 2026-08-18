#ifndef PAIR_HASHER_H
#define PAIR_HASHER_H
#include <functional>

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {
    template<typename T, typename U>
    struct pair_hasher_naive {
        static size_t operator()(const std::pair<T, U> &p) noexcept {
            return std::hash<T>{}(p.first) << 32 | std::hash<U>{}(p.second);
        }
    };

    template<class T>
    void hash_combine(std::size_t &seed, const T &v) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename T, typename U>
    struct pair_hasher {
        static size_t operator()(const std::pair<T, U> &p) noexcept {
            return hash_combine(std::hash<T>{}(p.first), std::hash<U>{}(p.second));
        }
    };
}

#endif //PAIR_HASHER_H
