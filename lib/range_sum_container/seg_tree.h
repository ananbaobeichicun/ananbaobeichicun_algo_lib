#ifndef SEG_TREE_H
#define SEG_TREE_H
#include <cassert>
#include <concepts>
#include <numeric>
#include <vector>
#include "segment.h"

template<typename T>
    requires requires(T t, size_t times)
    {
        { T{} } -> std::same_as<T>;
        { t + t } -> std::same_as<T>;
        { t += t } -> std::same_as<T &>;
        // { times * t } -> std::same_as<T>;
    }
struct seg_tree {
    struct node {
        segment<size_t> seg;
        T data, lazy;

        void update(const T &mod) {
            data += seg.len() * mod;
            lazy += mod;
        }
    };

    size_t _size;
    std::vector<node> nodes; // 1-based indexing
    using iterator_T = typename std::vector<node>::iterator;

private:
    constexpr bool is_leaf(const iterator_T p) {
        const size_t idx = p - nodes.begin();
        return idx >= _size;
    }

    constexpr iterator_T left(const iterator_T p) {
        assert(!is_leaf(p));
        const size_t idx = p - nodes.begin();
        return nodes.begin() + (idx << 1);
    }

    constexpr iterator_T right(const iterator_T p) {
        assert(!is_leaf(p));
        const size_t idx = p - nodes.begin();
        return nodes.begin() + (idx << 1 | 1);
    }

    void push_up(const iterator_T p) {
        if (!is_leaf(p))
            p->data = left(p)->data + right(p)->data;
    }

    void push_down(const iterator_T p) {
        if (!is_leaf(p)) {
            left(p)->update(p->lazy);
            right(p)->update(p->lazy);
        }
        p->lazy = T{};
    }

    template<std::ranges::random_access_range R>
        requires std::ranges::sized_range<R> && std::is_same_v<std::ranges::range_value_t<R>, T>
    void _inner_build(const R &v, const segment<size_t> seg, const iterator_T p)
    {
        p->seg = seg;
        if (is_leaf(p)) {
            p->data = seg.b < std::ranges::size(v) ? v[seg.b] : T{};
            return;
        }
        _inner_build(v, seg.left_half(), left(p));
        _inner_build(v, seg.right_half(), right(p));
        push_up(p);
    }

    T _inner_query(const segment<size_t> qseg, const iterator_T p) {
        if (segment<size_t>::is_disjoint(qseg, p->seg))
            return T{};
        if (p->seg <= qseg)
            return p->data;

        push_down(p);
        const T left_ans = _inner_query(qseg, left(p));
        const T right_ans = _inner_query(qseg, right(p));
        return left_ans + right_ans;
    }

    void _inner_modify(const segment<size_t> qseg, const T &mod, const iterator_T p) {
        if (segment<size_t>::is_disjoint(p->seg, qseg))
            return;
        if (p->seg <= qseg) {
            p->update(mod);
            return;
        }

        push_down(p);
        _inner_modify(qseg, mod, left(p));
        _inner_modify(qseg, mod, right(p));
        push_up(p);
    }

public:
    explicit seg_tree(const size_t n = 0): seg_tree(std::vector<T>(n)) {
    }

    template<std::ranges::random_access_range R>
        requires std::ranges::sized_range<R> && std::is_same_v<std::ranges::range_value_t<R>, T>
    explicit seg_tree(const R &v): _size(std::bit_ceil(v.size())), nodes(_size * 2) {
        _inner_build(v, {0, _size}, nodes.begin() + 1);
    }

    size_t size() const {
        return _size;
    }

    T query(const segment<size_t> seg) {
        assert(seg <= nodes[1].seg);
        return _inner_query(seg, nodes.begin() + 1);
    }

    void modify(const segment<size_t> seg, const T &mod) {
        assert(seg <= nodes[1].seg);
        _inner_modify(seg, mod, nodes.begin() + 1);
    }
};

template<std::ranges::random_access_range R>
    requires std::ranges::sized_range<R>
seg_tree(R) -> seg_tree<std::ranges::range_value_t<R>>;

#endif // SEG_TREE_H
