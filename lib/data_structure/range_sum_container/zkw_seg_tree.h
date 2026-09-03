#ifndef ZKW_SEG_TREE_H
#define ZKW_SEG_TREE_H
#include <vector>
#include <cassert>
#include <functional>
#include <bits/random.tcc>

namespace {
    // 要求T{}和G{}必须是单位元
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
            if (p < _size) data[p] = data[p << 1] + data[p << 1 | 1];
        }

        void push_down(const size_t p) {
            if (p < _size) {
                update(p << 1, tag[p]);
                update(p << 1 | 1, tag[p]);
            }
            tag[p] = T{};
        }

        void push_down_ancestors(const size_t p) {
            for (size_t i = (size_t) std::bit_width(_size) - 1; i; --i)
                push_down(p >> i);
        }

    public:
        explicit zkw_seg_tree(const size_t n) : _size(std::bit_ceil(n)), used_size(n), tag(_size << 1),
                                                data(_size << 1) {
        }

        template<std::ranges::range R>
            requires std::ranges::input_range<const R &>
                     && std::ranges::sized_range<const R &>
                     && std::indirectly_copyable<std::ranges::iterator_t<const R &>, typename std::vector<G>::iterator>
        explicit zkw_seg_tree(const R &r) : zkw_seg_tree(r.size()) {
            std::copy(r.begin(), r.end(), data.begin() + (ptrdiff_t) _size);
            for (size_t i = _size - 1; i; --i)
                data[i] = data[i << 1] + data[i << 1 | 1];
        }

        [[nodiscard]] size_t size() const { return used_size; }

        void modify(size_t x, const T &ntag) {
            assert(x < used_size);
            x += _size;
            push_down_ancestors(x);
            update(x, ntag);
            while (x >>= 1) push_up(x);
        }

        void modify(size_t l, size_t r, const T &ntag) {
            assert(l <= r && r <= used_size);
            l += _size;
            r += _size;
            push_down_ancestors(l);
            push_down_ancestors(r);

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

                do push_up(u >>= 1); while (l == r && u);
                do push_up(v >>= 1); while (l == r && v);
            }
        }

        G query(size_t x) {
            assert(x < used_size);
            x += _size;
            push_down_ancestors(x);
            return data[x];
        }

        G query(size_t l, size_t r) {
            assert(l <= r && r <= used_size);
            l += _size;
            r += _size;
            push_down_ancestors(l);
            push_down_ancestors(r);

            G ansl, ansr;
            for (; l < r; l >>= 1, r >>= 1) {
                if (l & 1) ansl = ansl + data[l++];
                if (r & 1) ansr = data[--r] + ansr;
            }

            return ansl + ansr;
        }

        // untested!!
        void resolve_all_tags() {
            for (size_t i = 1; i < _size; ++i) push_down(i);
            for (size_t i = _size - 1; i; --i) push_up(i);
        }

        // untested!!
        G &get_unchecked(const size_t idx) {
            return data[idx + _size];
        }

        // untested!!
        [[nodiscard]] const G &get_unchecked(const size_t idx) const {
            return data[idx + _size];
        }

        template<std::predicate<const G &> Fn>
        std::pair<size_t, G> max_right(const size_t l, Fn f) {
            if (l == used_size) return {used_size, G{}};
            assert(l <= used_size);
            size_t ll = l + _size;
            push_down_ancestors(ll);

            G acc;
            while (ll) {
                if (!std::invoke(f, acc + data[ll])) break;
                if (ll & 1) {
                    acc = acc + data[ll++];
                    if (std::has_single_bit(ll)) return {used_size, query(l, used_size)};
                } else {
                    ll >>= 1;
                }
            }

            while (ll < _size) {
                push_down(ll);
                if (std::invoke(f, acc + data[ll <<= 1])) acc = acc + data[ll++];
            }

            return {ll - _size, acc};
        }

        template<std::predicate<const G &> Fn>
        std::pair<size_t, G> min_left(const size_t r, Fn f) {
            if (!r) return {0, G{}};
            assert(r <= used_size);
            size_t rr = r + _size - 1;
            push_down_ancestors(rr);

            G acc;
            while (rr) {
                if (!std::invoke(f, data[rr] + acc)) break;
                if (!(rr & 1)) {
                    acc = data[rr--] + acc;
                    if (std::__is_power_of_2_less_1(rr)) return {0, query(0, r)};
                } else {
                    rr >>= 1;
                }
            }

            while (rr < _size) {
                push_down(rr);
                if (std::invoke(f, data[rr = rr << 1 | 1] + acc)) acc = data[rr--] + acc;
            }

            return {rr - _size + 1, acc};
        }
    };
}
#endif //ZKW_SEG_TREE_H
