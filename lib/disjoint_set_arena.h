#ifndef DISJOINT_SET_ARENA_H
#define DISJOINT_SET_ARENA_H

#include <numeric>
#include <vector>

struct disjoint_set {
    std::vector<size_t> par, _size;

    explicit disjoint_set(const size_t n): par(n), _size(n, 1) {
        std::iota(par.begin(), par.end(), 0);
    }

    size_t root(const size_t n) {
        return par[n] == n ? n : par[n] = root(par[n]);
    }

    size_t size(const size_t n) {
        return _size[root(n)];
    }

    bool link(const size_t x, const size_t y) {
        size_t rx = root(x), ry = root(y);
        if (rx == ry)
            return false;
        if (_size[rx] > _size[ry])
            std::swap(rx, ry);
        par[rx] = ry;
        _size[ry] += _size[rx];
        return true;
    }

    bool is_linked(const size_t x, const size_t y) {
        return root(x) == root(y);
    }
};

#endif //DISJOINT_SET_ARENA_H
