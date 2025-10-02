#ifndef SEG_TREE_H
#define SEG_TREE_H
#include <stdexcept>
#include <vector>

template<typename T, typename G>
requires requires(T t1, T t2, G g1, G g2)
{
    {t1 * t2} -> std::convertible_to<T>;
    {g1 + g2} -> std::convertible_to<G>;
    {t1 * g2} -> std::convertible_to<G>;
}
struct seg_tree {
private:
    size_t _size, used_size;
    std::vector<T> tag;
    std::vector<G> data;

    [[nodiscard]] std::pair<size_t, size_t> get_lr(const size_t p) const {
        const size_t l = std::bit_width(p) - 1;
        const size_t pp = p ^ 1uz << l;
        const size_t len = _size >> l;
        return {pp * len, (pp + 1) * len};
    }

    void update(const size_t p, const T &ntag) {
        data[p] = ntag * data[p];
        tag[p] = ntag * tag[p];
    }

    void push_up(const size_t p) {
        if (p < _size)
            data[p] = data[p << 1] + data[p << 1 | 1];
    }

    void push_down(const size_t p) {
        if (p < _size) {
            update(p << 1, tag[p]);
            update(p << 1 | 1, tag[p]);
        }
        tag[p] = T{};
    }

    void _modify(const size_t p, const size_t l, const size_t r, const T &ntag) {
        const auto [pl, pr] = get_lr(p);
        if (r <= pl || pr <= l)
            return;
        if (l <= pl && pr <= r) {
            update(p, ntag);
            return;
        }

        push_down(p);
        _modify(p << 1, l, r, ntag);
        _modify(p << 1 | 1, l, r, ntag);
        push_up(p);
    }

    G _query(const size_t p, const size_t l, const size_t r) {
        const auto [pl, pr] = get_lr(p);
        if (r <= pl || pr <= l)
            return G{};
        if (l <= pl && pr <= r)
            return data[p];

        push_down(p);
        G ans_l = _query(p << 1, l, r);
        G ans_r = _query(p << 1 | 1, l, r);
        return ans_l + ans_r;
    }

    size_t _right_partition_point(const size_t p, const size_t idx, G &acc, std::predicate<const G &> auto pred) {
        const auto [pl, pr] = get_lr(p);
        if (pr <= idx)
            return _size;
        if (pl >= idx) {
            if (pred(acc + data[p])) {
                acc = acc + data[p];
                return _size;
            }
            if (p >= _size)
                return pl;
        }

        push_down(p);
        size_t res = _right_partition_point(p << 1, idx, acc, pred);
        return res != _size ? res : _right_partition_point(p << 1 | 1, idx, acc, pred);
    }

    void _resolve_all_tags(const size_t p) {
        if (p >= _size)
            return;

        push_down(p);
        resolve_all_tags(p << 1);
        resolve_all_tags(p << 1 | 1);
    }

public:
    explicit seg_tree(const size_t n) : _size(std::bit_ceil(n)), used_size(n), tag(_size << 1), data(_size << 1) {
    }

    template<std::ranges::input_range R>
        requires std::ranges::sized_range<R>
                 && std::indirectly_copyable<std::ranges::iterator_t<R>, typename std::vector<G>::iterator>
    explicit seg_tree(const R &r) : seg_tree(r.size()) {
        std::copy(r.begin(), r.end(), data.begin() + _size);
        for (size_t i = _size - 1; i; --i)
            data[i] = data[i << 1] + data[i << 1 | 1];
    }

    [[nodiscard]] size_t size() const { return used_size; }

    void modify(const size_t l, const size_t r, const T &ntag) {
        if (l > r || r > used_size)
            throw std::range_error{""};
        _modify(1, l, r, ntag);
    }

    G query(const size_t l, const size_t r) {
        if (l > r || r > used_size)
            throw std::range_error{""};
        return _query(1, l, r);
    }

    size_t right_partition_point(const size_t idx, std::predicate<const G &> auto pred) {
        if (idx >= used_size)
            throw std::range_error{""};
        G acc;
        const size_t res = _right_partition_point(1, idx, acc, pred);
        return std::min(res, used_size);
    }

    // untested!!
    void resolve_all_tags() {
        resolve_all_tags(1);
    }

    // untested!!
    G &get_unchecked(const size_t idx) {
        return data[idx + _size];
    }
};
#endif //SEG_TREE_H