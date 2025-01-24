#ifndef SEG_TREE_H
#define SEG_TREE_H
#include <cassert>
#include <concepts>
#include <numeric>
#include <vector>
#include "segment.h"

static constexpr size_t overlap_length(segment x, segment y) {
    const size_t B = std::max(x.b, y.b), E = std::min(x.e, y.e);
    return B <= E ? E - B : 0;
}

template<typename T>
    requires requires(T t, size_t times)
    {
        { t + t } -> std::same_as<T>;
        { t += t } -> std::same_as<T &>;
        // { times * t } -> std::same_as<T>;
    }
struct seg_tree {
    size_t _size;
    std::vector<segment> segs;
    std::vector<T> data;
    std::vector<T> lazy;

    static constexpr size_t left(const size_t idx) {
        return ((idx + 1) << 1) - 1;
    }

    static constexpr size_t right(const size_t idx) {
        return ((idx + 1) << 1 | 1) - 1;
    }

private:
    void update(const size_t p, const T &mod) {
        data[p] += segs[p].len() * mod;
        lazy[p] += mod;
    }

    void push_up(const size_t p) {
        if (segs[p].len() > 1)
            data[p] = data[left(p)] + data[right(p)];
    }

    void push_down(const size_t p) {
        if (segs[p].len() > 1) {
            update(left(p), lazy[p]);
            update(right(p), lazy[p]);
        }
        lazy[p] = T{};
    }

    void _inner_build(const std::vector<T> &v, const segment seg, const size_t p) {
        segs[p] = seg;
        if (seg.len() == 1) {
            data[p] = seg.b < v.size() ? v[seg.b] : T{};
            return;
        }
        _inner_build(v, seg.left_half(), left(p));
        _inner_build(v, seg.right_half(), right(p));
        push_up(p);
    }

    T _inner_query(const segment qseg, const size_t p) {
        if (!overlap_length(qseg, segs[p]))
            return T{};
        if (segs[p].is_sub_interval_of(qseg))
            return data[p];
        push_down(p);
        T left_ans = _inner_query(qseg, left(p));
        T right_ans = _inner_query(qseg, right(p));
        return left_ans + right_ans;
    }

    void _inner_modify(const segment qseg, const T &mod, const size_t p) {
        if (p >= data.size())
            return;
        if (!overlap_length(qseg, segs[p]))
            return;
        if (segs[p].is_sub_interval_of(qseg)) {
            update(p, mod);
        } else {
            push_down(p);
            _inner_modify(qseg, mod, left(p));
            _inner_modify(qseg, mod, right(p));
            push_up(p);
        }
    }

public:
    explicit seg_tree(const size_t n = 0): _size(std::bit_ceil(n)), segs(_size * 2 - 1), data(_size * 2 - 1),
                                           lazy(_size * 2 - 1) {
    }

    explicit seg_tree(const std::vector<T> &v): seg_tree(v.size()) {
        _inner_build(v, {0, _size}, 0);
    }

    size_t size() const {
        return _size;
    }

    T query(const segment seg) {
        assert(seg.is_sub_interval_of(segs[0]));
        return _inner_query(seg, 0);
    }

    void modify(const segment seg, const T mod) {
        assert(seg.is_sub_interval_of(segs[0]));
        _inner_modify(seg, mod, 0);
    }
};
#endif // SEG_TREE_H
