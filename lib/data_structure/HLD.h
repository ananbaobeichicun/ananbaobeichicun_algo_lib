#ifndef HLD_H
#define HLD_H
#include <vector>
#include <cassert>
#include <algorithm>

struct HLD {
    std::vector<std::vector<size_t> > edges;
    size_t _size;
    std::vector<size_t> depth, fa, size, heavy_child;
    std::vector<size_t> head, dfn, inv_dfn;

    explicit HLD(std::vector<std::vector<size_t> > edges_) : edges(std::move(edges_)), _size(edges.size()),
                                                             depth(_size), fa(_size), size(_size, 1uz),
                                                             heavy_child(_size, ~0uz), head(_size), dfn(_size),
                                                             inv_dfn(_size) {
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

        for (size_t i = 0; i < _size; ++i)
            inv_dfn[dfn[i]] = i;
    }

    template<typename Fn>
        requires requires(Fn f, size_t u, size_t v)
        {
            { f(u, v) };
        }
    void seg_perform(size_t u, size_t v, Fn f) {
        assert(u < _size && v < _size);
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                f(dfn[head[u]], dfn[u] + 1);
                u = fa[head[u]];
            } else {
                f(dfn[head[v]], dfn[v] + 1);
                v = fa[head[v]];
            }
        }

        const auto [dfn_min, dfn_max] = std::minmax(dfn[u], dfn[v]);
        return f(dfn_min, dfn_max + 1);
    }

    template<typename Fn>
        requires requires(Fn f, size_t u, size_t v)
        {
            { f(u, v) };
        }
    void subtree_perform(const size_t idx, Fn f) {
        assert(idx < _size);
        return f(dfn[idx], dfn[idx] + size[idx]);
    }

    size_t LCA(const size_t u, const size_t v) {
        assert(u < _size && v < _size);
        size_t ans = ~0uz;
        seg_perform(u, v, [&](const size_t s, const size_t) {
            ans = s;
        });
        return inv_dfn[ans];
    }
};

#endif //HLD_H
