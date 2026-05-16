#ifndef PAINTING_H
#define PAINTING_H
#include <array>
#include <vector>

constexpr std::array<std::pair<ptrdiff_t, ptrdiff_t>, 4> dirs{{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

constexpr bool in_bound(const std::vector<std::vector<size_t> > &map, const size_t i, const size_t j) {
    if (map.empty()) [[unlikely]]
            return false;
    const size_t n = map.size(), m = map[0].size();
    return i < n && j < m;
}

void paint(std::vector<std::vector<size_t> > &map, const size_t target, const size_t dye, const size_t i,
           const size_t j) {
    if (!in_bound(map, i, j) || map[i][j] != target)
        return;
    map[i][j] = dye;
    for (const auto [di, dj]: dirs) {
        const auto ni = i + di, nj = j + dj;
        paint(map, target, dye, ni, nj);
    }
}

void paint_if(std::vector<std::vector<size_t> > &map, auto pred, const size_t dye, const size_t i,
              const size_t j) {
    const size_t n = map.size(), m = map[0].size();
    if (i >= n || j >= m || !pred(map[i][j]))
        return;
    map[i][j] = dye;
    for (const auto [di, dj]: dirs) {
        const auto ni = i + di, nj = j + dj;
        paint_if(map, pred, dye, ni, nj);
    }
}

#endif //PAINTING_H
