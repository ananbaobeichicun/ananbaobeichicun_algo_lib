#ifndef BIDIRECTIONAL_HLD_SEG_TREE_BINDER_H
#define BIDIRECTIONAL_HLD_SEG_TREE_BINDER_H
#include <vector>
#include <ranges>
#include <algorithm>
#include "../HLD.h"
#include "zkw_seg_tree.h"

namespace {
    template<typename T, typename G>
        requires requires(T t1, T t2, G g1, G g2)
        {
            { t1 * t2 } -> std::convertible_to<T>;
            { g1 + g2 } -> std::convertible_to<G>;
            { t1 * g2 } -> std::convertible_to<G>;
        }
    struct bidirectional_HLD_seg_tree_binder {
        HLD hld;
        zkw_seg_tree<T, G> forward_seg_tree, backward_seg_tree;

        explicit bidirectional_HLD_seg_tree_binder(std::vector<std::vector<size_t> > edges,
                                                   std::vector<G> data) : hld(std::move(edges)),
                                                                          forward_seg_tree(0uz),
                                                                          backward_seg_tree(0uz) {
            std::vector<G> transformed_data(hld._size);
            for (size_t i = 0; i < hld._size; ++i)
                transformed_data[hld.dfn[i]] = std::move(data[i]);

            forward_seg_tree = zkw_seg_tree<T, G>(transformed_data);
            backward_seg_tree = zkw_seg_tree<T, G>(transformed_data | std::views::reverse);
        }

        G seg_query(size_t u, size_t v) {
            assert(u < hld._size && v < hld._size);

            G lans, rans;
            hld.seg_perform(u, v, [&](const size_t s, const size_t t) {
                                lans = lans + backward_seg_tree.query(hld._size - t, hld._size - s);
                            }, [&](const size_t s, const size_t t) {
                                rans = forward_seg_tree.query(s, t) + rans;
                            });

            return lans + rans;
        }

        void seg_modify(size_t u, size_t v, const T &ntag) {
            assert(u < hld._size && v < hld._size);
            auto f = [&](const size_t s, const size_t t) {
                forward_seg_tree.modify(s, t, ntag);
                backward_seg_tree.modify(hld._size - t, hld._size - s, ntag);
            };
            hld.seg_perform(u, v, f, f);
        }

        G subtree_query(const size_t idx) {
            assert(idx < hld._size);

            G ans;
            hld.subtree_perform(idx, [&](const size_t s, const size_t t) {
                ans = ans + forward_seg_tree.query(s, t);
            });

            return ans;
        }

        void subtree_modify(const size_t idx, const T &ntag) {
            assert(idx < hld._size);

            hld.subtree_perform(idx, [&](const size_t s, const size_t t) {
                forward_seg_tree.modify(s, t, ntag);
                backward_seg_tree.modify(hld._size - t, hld._size - s, ntag);
            });
        }

        // untested!!
        void resolve_all_tags() {
            forward_seg_tree.resolve_all_tags();
            backward_seg_tree.resolve_all_tags();
        }

        G &get_forward_unchecked(const size_t idx) {
            return forward_seg_tree.get_unchecked(hld.dfn[idx]);
        }

        [[nodiscard]] const G &get_forward_unchecked(const size_t idx) const {
            return forward_seg_tree.get_unchecked(hld.dfn[idx]);
        }

        G &get_backward_unchecked(const size_t idx) {
            return backward_seg_tree.get_unchecked(hld._size - 1 - hld.dfn[idx]);
        }

        [[nodiscard]] const G &get_backward_unchecked(const size_t idx) const {
            return backward_seg_tree.get_unchecked(hld._size - 1 - hld.dfn[idx]);
        }
    };
}
#endif //BIDIRECTIONAL_HLD_SEG_TREE_BINDER_H
