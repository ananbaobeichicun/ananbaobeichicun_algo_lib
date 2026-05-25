#ifndef HLD_SEG_TREE_BINDER_H
#define HLD_SEG_TREE_BINDER_H
#include <vector>
#include <ranges>
#include <algorithm>
#include "../HLD.h"
#include "zkw_seg_tree.h"

template<typename T, typename G>
    requires requires(T t1, T t2, G g1, G g2)
    {
        { t1 * t2 } -> std::convertible_to<T>;
        { g1 + g2 } -> std::convertible_to<G>;
        { t1 * g2 } -> std::convertible_to<G>;
    }
struct forward_HLD {
    HLD hld;
    zkw_seg_tree<T, G> _seg_tree;

    explicit forward_HLD(std::vector<std::vector<size_t> > edges, std::vector<G> data) : hld(std::move(edges)),
        _seg_tree(0uz) {

        std::vector<G> transformed_data(hld._size);
        for (size_t i = 0; i < hld._size; ++i)
            transformed_data[hld.dfn[i]] = std::move(data[i]);

        _seg_tree = zkw_seg_tree<T, G>(transformed_data);
    }

    G seg_query(size_t u, size_t v) {
        assert(u < hld._size && v < hld._size);

        G ans;
        hld.seg_perform(u, v, [&] (const size_t s, const size_t t) {
            ans = ans + _seg_tree.query(s, t);
        });

        return ans;
    }

    void seg_modify(size_t u, size_t v, const T &ntag) {
        assert(u < hld._size && v < hld._size);

        hld.seg_perform(u, v, [&] (const size_t s, const size_t t) {
            _seg_tree.modify(s, t, ntag);
        });
    }

    G subtree_query(const size_t idx) {
        assert(idx < hld._size);

        G ans;
        hld.subtree_perform(idx, [&] (const size_t s, const size_t t) {
            ans = ans + _seg_tree.query(s, t);
        });

        return ans;
    }

    void subtree_modify(const size_t idx, const T &ntag) {
        assert(idx < hld._size);

        hld.seg_perform(idx, [&] (const size_t s, const size_t t) {
            _seg_tree.modify(s, t, ntag);
        });
    }

    // untested!!
    void resolve_all_tags() {
        _seg_tree.resolve_all_tags();
    }

    G &get_unchecked(const size_t idx) {
        return _seg_tree.get_unchecked(hld.dfn[idx]);
    }

    const G &get_unchecked(const size_t idx) const {
        return _seg_tree.get_unchecked(hld.dfn[idx]);
    }
};

#endif //HLD_SEG_TREE_BINDER_H
