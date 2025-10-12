#ifndef ZKW_SEG_TREE_H
#define ZKW_SEG_TREE_H
#include <stdexcept>
#include <vector>

template<typename T, typename G>
    requires requires(T t1, T t2, G g1, G g2)
    {
        { t1 * t2 } -> std::convertible_to<T>;
        { g1 + g2 } -> std::convertible_to<G>;
        { t1 * g2 } -> std::convertible_to<G>;
    }
struct zkw_seg_tree {
private:
    size_t _size, used_size;
    std::vector<T> tag;
    std::vector<G> data;

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

public:
    explicit zkw_seg_tree(const size_t n) : _size(std::bit_ceil(n)), used_size(n), tag(_size << 1), data(_size << 1) {
    }

    template<std::ranges::input_range R>
        requires std::ranges::sized_range<R>
                 && std::indirectly_copyable<std::ranges::iterator_t<R>, typename std::vector<G>::iterator>
    explicit zkw_seg_tree(const R &r) : zkw_seg_tree(r.size()) {
        std::copy(r.begin(), r.end(), data.begin() + _size);
        for (size_t i = _size - 1; i; --i)
            data[i] = data[i << 1] + data[i << 1 | 1];
    }

    [[nodiscard]] size_t size() const { return used_size; }

    void modify(size_t l, size_t r, const T &ntag) {
        if (l > r || r > used_size)
            throw std::exception{};
        l += _size;
        r += _size;
        for (size_t i = std::bit_width(_size) - 1; i; --i) {
            push_down(l >> i);
            if (r >> i != l >> i)
                push_down(r >> i);
        }

        size_t u, v;
        for (u = 0, v = 0; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                u = l;
                update(l++, ntag);
            }
            if (r & 1) {
                v = r;
                update(--r, ntag);
            }

            if (u)
                push_up(u >>= 1);
            if (v)
                push_up(v >>= 1);
        }

        while (u >> 1 || v >> 1) {
            push_up(u >>= 1);
            if (u >> 1 != v >> 1)
                push_up(v >>= 1);
        }
    }

    G query(size_t l, size_t r) {
        if (l > r || r > used_size)
            throw std::exception{};
        l += _size;
        r += _size;
        for (size_t i = std::bit_width(_size) - 1; i; --i) {
            push_down(l >> i);
            if (r >> i != l >> i)
                push_down(r >> i);
        }

        G ansl, ansr;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1)
                ansl = ansl + data[l++];
            if (r & 1)
                ansr = data[--r] + ansr;
        }

        return ansl + ansr;
    }
};
#endif //ZKW_SEG_TREE_H
