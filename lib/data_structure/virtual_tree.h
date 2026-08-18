#ifndef VIRTUAL_TREE_H
#define VIRTUAL_TREE_H
#include <unordered_map>
#include "HLD.h"

namespace {
    struct get_virtual_tree_result {
        std::vector<std::vector<size_t> > tree;
        std::unordered_map<size_t, size_t> idx_mapping;
        std::vector<size_t> inv_idx_mapping;
    };
}

static get_virtual_tree_result get_virtual_tree(const HLD &hld, std::vector<size_t> critical) {
    std::ranges::sort(critical, {}, [&](const size_t x) {
        return hld.dfn[x];
    });
    critical.erase(std::ranges::unique(critical).begin(), critical.end());

    get_virtual_tree_result ans{.tree = std::vector({std::vector<size_t>()}), .idx_mapping{}, .inv_idx_mapping{}};

    size_t time_stamp = 0;
    ans.idx_mapping[0] = time_stamp++;
    auto insert_son = [&](const size_t u, const size_t v) {
        ans.tree[ans.idx_mapping[u]].push_back(ans.idx_mapping[v]);
        ans.tree[ans.idx_mapping[v]].push_back(ans.idx_mapping[u]);
    };

    std::vector<size_t> stk({0});
    for (const size_t x: critical) {
        if (!x)
            continue;
        size_t lca = hld.LCA(x, stk.back());
        size_t prev_top = ~0uz;
        while (hld.LCA(x, stk.back()) != stk.back()) {
            if (~prev_top)
                insert_son(stk.back(), prev_top);

            prev_top = stk.back();
            stk.pop_back();
        }
        if (lca != stk.back()) {
            ans.idx_mapping[lca] = time_stamp++;
            ans.tree.emplace_back();
            stk.push_back(lca);
        }
        if (~prev_top)
            insert_son(lca, prev_top);
        ans.idx_mapping[x] = time_stamp++;
        ans.tree.emplace_back();
        stk.push_back(x);
    }

    size_t prev_top = ~0uz;
    while (!stk.empty()) {
        if (~prev_top)
            insert_son(stk.back(), prev_top);
        prev_top = stk.back();
        stk.pop_back();
    }

    ans.inv_idx_mapping.resize(time_stamp);
    for (const auto [k, v]: ans.idx_mapping)
        ans.inv_idx_mapping[v] = k;

    return ans;
}

#endif //VIRTUAL_TREE_H
