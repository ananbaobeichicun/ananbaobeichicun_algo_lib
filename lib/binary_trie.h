#ifndef BINARY_TRIE_H
#define BINARY_TRIE_H
#include "array_based_full_binary_tree.h"

struct trie_node_data {
    bool is_valid = false;
    bool is_complete_word = false;
};

template<size_t max_len>
class binary_trie {
    constexpr static size_t depth_max = max_len;
    using ull = unsigned long long;
    using data_t = ABFBT<trie_node_data, max_len + 1>;
    using iter_t = ABFBT_iter<trie_node_data, max_len + 1>;
    data_t data; // 大端序
public:
    binary_trie() {
        data[1].is_valid = true;
    }

    void insert(const ull s) {
        iter_t it(data);
        for (size_t i = 1 << depth_max - 1; i; i >>= 1) {
            if (s & i)
                it >>= 0;
            else
                it <<= 0;
            it->is_valid = true;
        }
        it->is_complete_word = true;
    }

    ull search_fartherest(const ull s) {
        iter_t it(data);
        for (size_t i = 1 << depth_max - 1; i; i >>= 1) {
            if (s & i) {
                if ((it << 0)->is_valid)
                    it <<= 0;
                else
                    it >>= 0;
            } else {
                if ((it >> 0)->is_valid)
                    it >>= 0;
                else
                    it <<= 0;
            }
        }
        return it.get_index() & ((1 << max_len) - 1);
    }
};
#endif //BINARY_TRIE_H
