#ifndef SEGMENT_H
#define SEGMENT_H
#include <numeric>
#include <optional>
#include <variant>
#include <utility>
#include <algorithm>

template<typename T>
struct segment;

template<typename T>
std::partial_ordering operator<=>(segment<T>, segment<T>);

enum class segment_relationship {
    equal, intersect_left_of, intersect_right_of, disjoint_left_of, disjoint_right_of, is_subset_of, is_superset_of
};

template<typename T>
struct segment {
private:
    friend std::partial_ordering operator<=><T>(segment, segment);

public:
    T b, e;

    constexpr segment() = default;

    constexpr segment(const T b, const T e): b(b), e(e) {
        if (b > e)
            throw "bad segment initialization";
    }

    constexpr static std::optional<segment> try_construct(const T b, const T e) {
        return b <= e ? std::optional{segment{b, e}} : std::nullopt;
    }

    constexpr T len() const {
        return e - b;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    constexpr operator bool() const {
        return e != b;
    }

    constexpr bool contains(T x) const {
        return b <= x && x < e;
    }

    constexpr std::pair<segment, segment> split(T x) const {
        return {{b, x}, {x, e}};
    }

    constexpr T mid() const {
        return std::midpoint(b, e);
    }

    constexpr std::pair<segment, segment> bisect() const {
        return split(mid());
    }

    constexpr segment left_half () const {
        return bisect().first;
    }

    constexpr segment right_half () const {
        return bisect().second;
    }

    constexpr static segment_relationship relationship(const segment x, const segment y) {
        if (x.b == y.b && x.e == y.e)
            return segment_relationship::equal;
        if (x.b < y.b && y.b < x.e && x.e < y.e)
            return segment_relationship::intersect_left_of;
        if (y.b < x.b && x.b < y.e && y.e < x.e)
            return segment_relationship::intersect_right_of;
        if (x.e <= y.b)
            return segment_relationship::disjoint_left_of;
        if (y.e <= x.b)
            return segment_relationship::disjoint_right_of;
        if (y.b <= x.b && x.e <= y.e)
            return segment_relationship::is_subset_of;
        if (x.b <= y.b && y.e <= x.e)
            return segment_relationship::is_superset_of;
        std::unreachable();
    }

    constexpr static bool is_disjoint(const segment x, const segment y) {
        const segment_relationship rel = relationship(x, y);
        return rel == segment_relationship::disjoint_left_of || rel == segment_relationship::disjoint_right_of;
    }
};

template<typename T>
std::partial_ordering operator<=>(const segment<T> x, const segment<T> y) {
    const segment_relationship rel = segment<T>::relationship(x, y);
    if (rel == segment_relationship::equal)
        return std::partial_ordering::equivalent;
    if (rel == segment_relationship::is_subset_of)
        return std::partial_ordering::less;
    if (rel == segment_relationship::is_superset_of)
        return std::partial_ordering::greater;
    return std::partial_ordering::unordered;
}

template<typename T>
constexpr std::optional<segment<T> > operator&(const segment<T> x, const segment<T> y) {
    const T B = std::max(x.b, y.b);
    const T E = std::min(x.e, y.e);
    return segment<T>::try_construct(B, E);
}

//fixme
template<typename T>
constexpr std::variant<segment<T>, std::pair<segment<T>, segment<T> > > operator|(
    const segment<T> x, const segment<T> y) {
    switch (segment<T>::relationship(x, y)) {
        case segment_relationship::equal:
        case segment_relationship::is_subset_of:
            return y;
        case segment_relationship::is_superset_of:
            return x;
        case segment_relationship::intersect_left_of:
            return {x.b, y.e};
        case segment_relationship::intersect_right_of:
            return {y.b, x.e};
        case segment_relationship::disjoint_left_of:
            return {x, y};
        case segment_relationship::disjoint_right_of:
            return {y, x};
        default:
            std::unreachable();
    }
}

template<typename T>
constexpr std::pair<segment<T>, segment<T> > operator^(const segment<T> x, const segment<T> y) {
    if (x <= y)
        return {{y.b, x.b}, {x.e, y.e}};
    if (y <= x)
        return {{x.b, y.b}, {y.e, x.e}};
    if (x.e <= y.b)
        return {x, y};
    if (y.e <= x.b)
        return {y, x};
    return {{std::min(x.b, y.b), std::max(x.b, y.b)}, {std::min(x.e, y.e), std::max(x.e, y.e)}};
}

template<typename T>
constexpr std::optional<std::variant<segment<T>, std::pair<segment<T>, segment<T> > > > operator/(
    const segment<T> x, const segment<T> y) {
    if (x < y)
        return std::nullopt;
    if (y <= x)
        return {{x.b, y.b}, {y.e, x.e}};
    if (segment<T>::is_disjoint(x, y))
        return x;
    return x.b < y.b ? segment<T>{x.b, y.b} : segment<T>{y.e, x.e};
}
#endif //SEGMENT_H
