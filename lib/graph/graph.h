#ifndef GRAPH_H
#define GRAPH_H
#include <queue>
#include <unordered_set>

template<typename T, typename U>
class graph {
public:
    using node_size_t = size_t;
    using edge_size_t = size_t;

    virtual T &node(size_t index);

    [[nodiscard]] virtual size_t node_num() const;

    virtual U &edge(node_size_t src, node_size_t dst);

    virtual std::vector<node_size_t> out_node_connected_to(node_size_t src);

    virtual ~graph() = default;

    //todo test
    std::vector<U> dijkstra(size_t src) {
        std::vector<U> dis(node_num());
        std::unordered_set<node_size_t> first_upded({src});
        // std::vector<unsigned char> vis(node_num());
        struct to_upd_cell {
            node_size_t index;
            U dist;
        };
        std::priority_queue<to_upd_cell> to_upd(
            [](const to_upd_cell x, const to_upd_cell y) -> bool { return x.dist > y.dist; },
            {node(src)}); //todo extract priority_queue_set
        std::unordered_set<node_size_t> vis({src});

        dis[src] = 0;
        to_upd.push(src);

        while (!to_upd.empty()) {
            to_upd_cell cur = to_upd.top();
            vis.insert(cur.index);
            to_upd.pop();

            for (node_size_t i: out_node_connected_to(cur.index)) {
                if (vis.contains(i))
                    continue;
                if (dis[i] > dis[cur.index] + edge(cur.index, i) || !first_upded.contains(i)) {
                    first_upded.insert(i);
                    dis[i] = dis[cur.index] + edge(cur.index, i);
                    to_upd.push(i);
                }
            }
        }

        return dis;
    }
};

#endif //GRAPH_H
