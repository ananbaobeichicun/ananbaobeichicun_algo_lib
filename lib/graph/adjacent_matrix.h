#ifndef ADJACENT_MATRIX_H
#define ADJACENT_MATRIX_H
#include <vector>

#include "graph.h"

template<typename T, typename U>
class adjacent_matrix final : graph<T, U> {
    std::vector<T> nodes;
    std::vector<std::vector<U> > edges;

    T &node(size_t index) override {
        return node[index];
    }

    U &edge(size_t src, size_t dst) override {
        return edges[src][dst];
    }

};

#endif //ADJACENT_MATRIX_H
