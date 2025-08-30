#ifndef MAPPING_VECTOR_H
#define MAPPING_VECTOR_H
#include <functional>
#include <vector>

template<typename K, typename V, auto f>
    requires requires(K k)
    {
        { f(k) } -> std::same_as<size_t>;
    }
struct mapping_vector : std::vector<V> {
    using std::vector<V>::vector;

    // [[no_unique_address]] F f = F{};

    V &operator[](const K &idx) {
        return std::vector<V>::operator[](f(idx));
    }

    const V &operator[](const K &idx) const {
        return std::vector<V>::operator[](f(idx));
    }
};
#endif //MAPPING_VECTOR_H
