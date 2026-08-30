#ifndef TARJAN_SCC_H
#define TARJAN_SCC_H
#include <vector>
#include <unordered_set>
#include "adjacency_list_graph.h"

namespace {
    struct tarjan_SCC {
        const adjacency_list_graph<void> &graph;
        const size_t _size;
        std::vector<size_t> stk, dfn, low;
        std::vector<std::vector<size_t> > groups;
        std::vector<size_t> node_mapping;
        std::vector<std::unordered_set<size_t> > nedges;

        tarjan_SCC(const adjacency_list_graph<void> &edges) : graph(edges), _size(edges.size()), dfn(_size, ~0uz),
                                                                 low(_size, ~0uz), node_mapping(_size) {
            if (!_size)
                return;
            std::vector<uint8_t> instk(_size, 0);
            size_t timestamp = 0;

            auto dfs = [&](auto &&self, const size_t cur) -> void {
                dfn[cur] = low[cur] = timestamp++;
                stk.push_back(cur);
                instk[cur] = true;
                for (const size_t nxt: graph.edges[cur]) {
                    if (!~dfn[nxt]) {
                        self(self, nxt);
                        low[cur] = std::min(low[cur], low[nxt]);
                    } else if (instk[nxt]) {
                        low[cur] = std::min(low[cur], dfn[nxt]);
                    }
                }

                if (dfn[cur] == low[cur]) {
                    groups.emplace_back();
                    while (stk.back() != cur) {
                        groups.back().push_back(stk.back());
                        node_mapping[stk.back()] = groups.size() - 1;
                        instk[stk.back()] = false;
                        stk.pop_back();
                    }
                    groups.back().push_back(stk.back());
                    node_mapping[stk.back()] = groups.size() - 1;
                    instk[stk.back()] = false;
                    stk.pop_back();
                }
            };

            for (size_t i = 0; i < _size; ++i)
                if (!~dfn[i])
                    dfs(dfs, i);

            nedges.resize(groups.size());
            for (size_t u = 0; u < _size; ++u)
                for (const size_t v: graph.edges[u])
                    if (node_mapping[u] != node_mapping[v])
                        nedges[node_mapping[u]].insert(node_mapping[v]);
        }
    };
}

#endif //TARJAN_SCC_H
