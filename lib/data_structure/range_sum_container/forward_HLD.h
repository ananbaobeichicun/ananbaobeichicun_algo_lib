#ifndef FORWARD_HLD_H
#define FORWARD_HLD_H
#include <stdexcept>
#include <vector>
#include <ranges>
#include "zkw_seg_tree.h"

template<typename T, typename G>
    requires requires(T t1, T t2, G g1, G g2)
    {
        { t1 * t2 } -> std::convertible_to<T>;
        { g1 + g2 } -> std::convertible_to<G>;
        { t1 * g2 } -> std::convertible_to<G>;
    }
struct forward_HLD {
    std::vector<std::vector<size_t> > edges;
    size_t _size;
    std::vector<size_t> depth, fa, size, heavy_child;
    std::vector<size_t> head, dfn;
    zkw_seg_tree<T, G> _seg_tree;

    explicit forward_HLD(std::vector<std::vector<size_t> > &&edges_,
                 std::vector<G> &&data) : edges(std::move(edges_)),
                                          _size(edges.size()), depth(_size),
                                          fa(_size), size(_size, 1uz),
                                          heavy_child(_size, ~0uz), head(_size),
                                          dfn(_size), _seg_tree(0uz) {
        auto dfs1 = [&](auto &&self, const size_t cur, const size_t from) -> void {
            depth[cur] = ~from ? depth[from] + 1 : 0;
            fa[cur] = ~from ? from : ~0uz;
            for (const size_t nxt: edges[cur]) {
                if (nxt != from) {
                    self(self, nxt, cur);
                    size[cur] += size[nxt];
                    if (!~heavy_child[cur] || size[nxt] > size[heavy_child[cur]])
                        heavy_child[cur] = nxt;
                }
            }
        };
        dfs1(dfs1, 0uz, ~0uz);

        auto dfs2 = [&, time_stamp = 0uz](auto &&self, const size_t cur, const size_t cur_head) mutable -> void {
            dfn[cur] = time_stamp++;
            head[cur] = cur_head;
            if (~heavy_child[cur])
                self(self, heavy_child[cur], cur_head);
            for (const size_t nxt: edges[cur])
                if (nxt != fa[cur] && nxt != heavy_child[cur])
                    self(self, nxt, nxt);
        };
        dfs2(dfs2, 0uz, 0uz);

        std::vector<G> transformed_data(_size);
        for (size_t i = 0; i < _size; ++i)
            transformed_data[dfn[i]] = std::move(data[i]);

        _seg_tree = zkw_seg_tree<T, G>(transformed_data);
    }

    G seg_query(size_t u, size_t v) {
        if (u >= _size || v >= _size)
            throw std::range_error{""};
        G ans;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                ans = ans + _seg_tree.query(dfn[head[u]], dfn[u] + 1);
                u = fa[head[u]];
            } else {
                ans = ans + _seg_tree.query(dfn[head[v]], dfn[v] + 1);
                v = fa[head[v]];
            }
        }
        if (depth[u] > depth[v])
            std::swap(u, v);
        return ans + _seg_tree.query(dfn[u], dfn[v] + 1);
    }

    void seg_modify(size_t u, size_t v, const T &ntag) {
        if (u >= _size || v >= _size)
            throw std::range_error{""};
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                _seg_tree.modify(dfn[head[u]], dfn[u] + 1, ntag);
                u = fa[head[u]];
            } else {
                _seg_tree.modify(dfn[head[v]], dfn[v] + 1, ntag);
                v = fa[head[v]];
            }
        }

        const size_t dfn_min = std::min(dfn[u], dfn[v]);
        const size_t dfn_max = std::max(dfn[u], dfn[v]);

        _seg_tree.modify(dfn_min, dfn_max + 1, ntag);
    }

    G subtree_query(const size_t idx) {
        return _seg_tree.query(dfn[idx], dfn[idx] + size[idx]);
    }

    void subtree_modify(const size_t idx, const T &ntag) {
        _seg_tree.modify(dfn[idx], dfn[idx] + size[idx], ntag);
    }

    // untested!!
    void resolve_all_tags() {
        _seg_tree.resolve_all_tags();
    }

    G &get_unchecked(const size_t idx) {
        return _seg_tree.get_unchecked(dfn[idx]);
    }

    const G &get_unchecked(const size_t idx) const {
        return _seg_tree.get_unchecked(dfn[idx]);
    }
};

#endif //FORWARD_HLD_H
