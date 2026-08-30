#ifndef ADJACENCY_LIST_GRAPH_H
#define ADJACENCY_LIST_GRAPH_H
#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cassert>
#include <cstdint>
#include <unordered_set>

namespace {
    template<typename T>
    struct adjacency_list_graph {
        std::vector<std::vector<std::pair<size_t, T> > > edges;

        explicit adjacency_list_graph(const size_t n) : edges(n) {
        }

        explicit adjacency_list_graph(std::vector<std::vector<std::pair<size_t, T> > > edges) : edges(std::move(edges)) {
        }

        [[nodiscard]] size_t size() const {
            return edges.size();
        }

        void check_validity() const {
            for (size_t i = 0; i < edges.size(); ++i)
                for (const size_t j: edges[i] | std::views::keys)
                    assert(j < edges.size());
        }

        static adjacency_list_graph read1(const size_t n, const size_t m) {
            adjacency_list_graph ans(n);
            for (size_t i = 0; i < m; ++i) {
                size_t u, v;
                T val;
                std::cin >> u >> v >> val;
                --u, --v;
                ans.edges[u].emplace_back(v, val);
            }
            return ans;
        }

        static adjacency_list_graph read1_bidirectional(const size_t n, const size_t m) {
            adjacency_list_graph ans(n);
            for (size_t i = 0; i < m; ++i) {
                size_t u, v;
                T val;
                std::cin >> u >> v >> val;
                --u, --v;
                ans.edges[u].emplace_back(v, val);
                ans.edges[v].emplace_back(u, val);
            }
            return ans;
        }

        [[nodiscard]] bool is_simple() const {
            for (size_t i = 0; i < edges.size(); ++i) {
                std::unordered_set<size_t> bucket;
                for (const size_t j: edges[i] | std::views::keys)
                    if (!bucket.insert(j).second)
                        return false;
                if (bucket.contains(i))
                    return false;
            }
            return true;
        }

        [[nodiscard]] adjacency_list_graph inv() const {
            adjacency_list_graph ans(edges.size());
            for (size_t i = 0; i < edges.size(); ++i)
                for (const auto &[j, v]: edges[i])
                    ans.edges[j].emplace_back(i, v);
            return ans;
        }

        [[nodiscard]] adjacency_list_graph remap_nodes(std::vector<size_t> node_mapping) const {
            assert(node_mapping.size() == edges.size());
            std::vector<uint8_t> vis(edges.size());
            for (size_t i = 0; i < edges.size(); ++i) {
                assert(!~node_mapping[i] || node_mapping[i] < edges.size());
                if (~node_mapping[i]) {
                    assert(!vis[node_mapping[i]]);
                    vis[node_mapping[i]] = true;
                }
            }
            assert(std::find(std::find(vis.begin(), vis.end(), false), vis.end(), true) == vis.end());

            adjacency_list_graph ngraph(std::ranges::count_if(node_mapping, [](const size_t x) { return ~x; }));

            for (size_t i = 0; i < edges.size(); ++i)
                if (~node_mapping[i])
                    ngraph.edges[node_mapping[i]] = edges[i];

            for (auto &e: ngraph.edges) {
                for (size_t i = 0; i < e.size();) {
                    if (~node_mapping[e[i].first]) {
                        e[i].first = node_mapping[e[i].first];
                        ++i;
                    } else {
                        std::swap(e[i], e.back());
                        e.pop_back();
                    }
                }
            }

            return ngraph;
        }
    };

    template<>
    struct adjacency_list_graph<void> {
        std::vector<std::vector<size_t> > edges;

        explicit adjacency_list_graph(const size_t n) : edges(n) {
        }

        explicit adjacency_list_graph(std::vector<std::vector<size_t> > edges) : edges(std::move(edges)) {
        }

        [[nodiscard]] size_t size() const {
            return edges.size();
        }

        void check_validity() const {
            for (size_t i = 0; i < edges.size(); ++i)
                for (const size_t j: edges[i])
                    assert(j < edges.size());
        }

        static adjacency_list_graph read1(const size_t n, const size_t m) {
            adjacency_list_graph ans(n);
            for (size_t i = 0; i < m; ++i) {
                size_t u, v;
                std::cin >> u >> v;
                --u, --v;
                ans.edges[u].emplace_back(v);
            }
            return ans;
        }

        static adjacency_list_graph read1_bidirectional(const size_t n, const size_t m) {
            adjacency_list_graph ans(n);
            for (size_t i = 0; i < m; ++i) {
                size_t u, v;
                std::cin >> u >> v;
                --u, --v;
                ans.edges[u].emplace_back(v);
                ans.edges[v].emplace_back(u);
            }
            return ans;
        }

        [[nodiscard]] bool is_simple() const {
            for (size_t i = 0; i < edges.size(); ++i) {
                std::unordered_set<size_t> bucket;
                for (const size_t j: edges[i])
                    if (!bucket.insert(j).second)
                        return false;
                if (bucket.contains(i))
                    return false;
            }
            return true;
        }

        [[nodiscard]] adjacency_list_graph inv() const {
            adjacency_list_graph ans(edges.size());
            for (size_t i = 0; i < edges.size(); ++i)
                for (const size_t j: edges[i])
                    ans.edges[j].emplace_back(i);
            return ans;
        }

        [[nodiscard]] adjacency_list_graph remap_nodes(std::vector<size_t> node_mapping) const {
            assert(node_mapping.size() == edges.size());
            std::vector<uint8_t> vis(edges.size());
            for (size_t i = 0; i < edges.size(); ++i) {
                assert(!~node_mapping[i] || node_mapping[i] < edges.size());
                if (~node_mapping[i]) {
                    assert(!vis[node_mapping[i]]);
                    vis[node_mapping[i]] = true;
                }
            }
            assert(std::find(std::find(vis.begin(), vis.end(), false), vis.end(), true) == vis.end());

            adjacency_list_graph ngraph(std::ranges::count_if(node_mapping, [](const size_t x) { return ~x; }));

            for (size_t i = 0; i < edges.size(); ++i)
                if (~node_mapping[i])
                    ngraph.edges[node_mapping[i]] = edges[i];

            for (auto &e: ngraph.edges) {
                for (size_t i = 0; i < e.size();) {
                    if (~node_mapping[e[i]]) {
                        e[i] = node_mapping[e[i]];
                        ++i;
                    } else {
                        std::swap(e[i], e.back());
                        e.pop_back();
                    }
                }
            }

            return ngraph;
        }
    };
}

#endif //ADJACENCY_LIST_GRAPH_H
