#ifndef SEGMENT_H
#define SEGMENT_H
#include <cassert>
#include <numeric>

struct segment {
    size_t b, e;

    segment() = default;

    segment(const size_t b, const size_t e): b(b), e(e) {
        assert(b <= e);
    }

    constexpr bool is_sub_interval_of(const segment other) const {
        return other.b <= b && e <= other.e;
    }

    constexpr size_t len() const {
        return e - b;
    }

    constexpr size_t mid() const {
        return std::midpoint(b, e);
    }

    constexpr segment left_half() const {
        return {b, mid()};
    }

    constexpr segment right_half() const {
        return {mid(), e};
    }
};
#endif //SEGMENT_H
