#ifndef GRAPH_UTIL_H
#define GRAPH_UTIL_H
#include <stack>
#include <vector>

template<typename T>
using edges_t = std::vector<std::pair<std::vector<size_t>, T> >;

/**
 * @brief Performs a depth-first search to find a path from a start node to a destination node.
 *
 * Recursively explores the graph represented as an adjacency list. The function records the indices of edges taken in
 * the @p path vector. To prevent immediately backtracking along the same edge, the parameter @p from is used to ignore
 * the previous node.
 *
 * @tparam T The type of additional data stored with each edge (e.g., weight, label).
 * @param[out] path On successful return, contains the sequence of edge indices from @p cur to @p dst.
 *                  If the function returns false, the contents of @p path will not be mutated.
 * @param[in] edges The graph's adjacency list. For node i, edges[i] is a list of pairs
 *                  (neighbor node index, edge data).
 * @param[in] dst   The index of the destination node.
 * @param[in] cur   The index of the current node being explored.
 * @param[in] from  The index of the node we came from in the previous recursive call.
 *                  This parameter is used to avoid immediate backtracking. Defaults to ~0uz representing “no previous node.”
 * @return True if a path from @p cur to @p dst exists (and @p path is filled accordingly);
 *         false otherwise.
 */
template<typename T>
bool search(std::vector<size_t> &path, const edges_t<T> &edges, const size_t dst, const size_t cur,
            const size_t from = ~0uz) {
    if (cur == dst)
        return true;
    for (size_t i = 0; i < edges[cur].size(); ++i) {
        const auto nxt = edges[cur][i].first;
        if (nxt != from) {
            path.push_back(i);
            if (search(path, edges, nxt, cur, dst))
                return true;
            path.pop_back();
        }
    }
    return false;
}

#endif //GRAPH_UTIL_H
