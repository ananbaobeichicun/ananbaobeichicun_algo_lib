#ifndef UNTITLED_HLD_H
#define UNTITLED_HLD_H
#include <stdexcept>
#include <vector>
#include <ranges>
#include "seg_tree.h"

template<typename T, typename G>
struct HLD {
    std::vector<std::vector<size_t> > edges;
    size_t _size;
    std::vector<size_t> depth, fa, size, heavy_child;
    std::vector<size_t> head, dfn, inv_dfn;
    seg_tree<T, G> forward_seg_tree, backward_seg_tree;

    explicit HLD(std::vector<std::vector<size_t> > &&edges_,
                 std::vector<G> &&data) : edges(std::move(edges_)),
                                          _size(edges.size()), depth(_size),
                                          fa(_size), size(_size, 1uz),
                                          heavy_child(_size, ~0uz), head(_size),
                                          dfn(_size), inv_dfn(_size), forward_seg_tree(0uz), backward_seg_tree(0uz) {
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
            dfn[cur] = time_stamp;
            inv_dfn[time_stamp++] = cur;
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

        forward_seg_tree = seg_tree<T, G>(transformed_data);
        backward_seg_tree = seg_tree<T, G>(transformed_data | std::views::reverse);
    }

    G seg_query(size_t u, size_t v) {
        if (u >= _size || v >= _size)
            throw std::range_error{""};
        G ans_u, ans_v;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                ans_u = ans_u + backward_seg_tree.query(_size - 1 - dfn[u], _size - 1 - dfn[head[u]] + 1);
                u = fa[head[u]];
            } else {
                ans_v = forward_seg_tree.query(dfn[head[v]], dfn[v] + 1) + ans_v;
                v = fa[head[v]];
            }
        }
        const G ans_mid = depth[u] < depth[v]
                              ? forward_seg_tree.query(dfn[u], dfn[v] + 1)
                              : backward_seg_tree.query(_size - 1 - dfn[u], _size - 1 - dfn[v] + 1);
        return ans_u + ans_mid + ans_v;
    }

    void seg_modify(size_t u, size_t v, const T &ntag) {
        if (u >= _size || v >= _size)
            throw std::range_error{""};
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                forward_seg_tree.modify(dfn[head[u]], dfn[u] + 1, ntag);
                backward_seg_tree.modify(_size - 1 - dfn[u], _size - 1 - dfn[head[u]] + 1, ntag);
                u = fa[head[u]];
            } else {
                forward_seg_tree.modify(dfn[head[v]], dfn[v] + 1, ntag);
                backward_seg_tree.modify(_size - 1 - dfn[v], _size - 1 - dfn[head[v]] + 1, ntag);
                v = fa[head[v]];
            }
        }

        const size_t dfn_min = std::min(dfn[u], dfn[v]);
        const size_t dfn_max = std::max(dfn[u], dfn[v]);

        forward_seg_tree.modify(dfn_min, dfn_max + 1, ntag);
        backward_seg_tree.modify(_size - 1 - dfn_max, _size - 1 - dfn_min + 1, ntag);
    }

    // it's unsound to call this function when (G, +) is not communicative monoid
    G subtree_query(const size_t idx) {
        return forward_seg_tree.query(dfn[idx], dfn[idx] + size[idx]);
    }

    void subtree_modify(const size_t idx, const T &ntag) {
        forward_seg_tree.modify(dfn[idx], dfn[idx] + size[idx], ntag);
        backward_seg_tree.modify(_size - 1 - (dfn[idx] + size[idx] - 1), _size - 1 - (dfn[idx] - 1), ntag);
    }
};

#endif //UNTITLED_HLD_H
