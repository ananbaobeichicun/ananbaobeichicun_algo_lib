#ifndef BINARY_INDEXED_TREE_H
#define BINARY_INDEXED_TREE_H
#include <numeric>
#include <vector>

constexpr static struct from_origin_t {
} from_origin_tag;

constexpr static struct from_sum_t {
} from_sum_tag;

//the default value of T should be identity element
template<typename T>
    requires requires(T t)
    {
        { t + t } -> std::same_as<T>;
        { t += t } -> std::same_as<T &>;
    }
struct binary_indexed_tree {
    std::vector<T> data;

    constexpr binary_indexed_tree() = default;

    explicit constexpr binary_indexed_tree(const size_t n): data(n) {
    }

    // //input origin array in O(n)
    // template<std::ranges::input_range R>
    //     requires requires(T t)
    //     {
    //         { t - t } -> std::same_as<T>;
    //     } && std::is_same_v<std::ranges::range_value_t<R>, T>
    // explicit constexpr binary_indexed_tree(const R &v): data(v.size()) {
    //     std::vector<T> sum(v.size());
    //     for (auto it = v.begin(); it != v.end(); ++it) {
    //         const size_t idx = it - v.begin();
    //         sum[idx] = (idx ? sum[idx - 1] : T{}) + *it;
    //         data[idx] = sum[idx] - (idx & idx + 1 ? sum[(idx & idx + 1) - 1] : T{});
    //     }
    // }

    //input origin array in O(n)
    template<std::ranges::range R>
        requires requires(T t)
                 {
                     { t - t } -> std::same_as<T>;
                 } && std::is_same_v<std::ranges::range_value_t<R>, T>
                 && std::ranges::input_range<R>
    explicit constexpr binary_indexed_tree(const R &v, from_origin_t): data(v.size()) {
        std::vector<T> sum;
        sum.reserve(v.size());
        std::partial_sum(v.begin(), v.end(), std::back_inserter(sum));
        *this = binary_indexed_tree(sum, from_sum_tag);
    }

    template<std::ranges::range R>
        requires requires(T t)
                 {
                     { t - t } -> std::same_as<T>;
                 } && std::is_same_v<std::ranges::range_value_t<R>, T>
                 && std::ranges::input_range<R>
                 && std::ranges::sized_range<R>
                 && std::ranges::random_access_range<R>
    explicit constexpr binary_indexed_tree(const R &v, from_sum_t): data(v.size()) {
        for (size_t i = 0; i < v.size(); ++i) {
            data[i] = v[i] - (i & i + 1 ? v[(i & i + 1) - 1] : T{});
        }
    }

    constexpr std::vector<T> into_sum() {
        std::vector<T> ans(data.size());
        for (size_t idx = 0; idx < data.size(); ++idx)
            ans[idx] = data[idx] + (idx & idx + 1 ? ans[(idx & idx + 1) - 1] : T{});
        return ans;
    }

    constexpr size_t size() const {
        return data.size();
    }

    constexpr void add(size_t idx, const T &delta) {
        for (; idx < data.size(); idx |= idx + 1)
            data[idx] += delta;
    }

    [[nodiscard]] constexpr T sum(size_t idx) const {
        T ans;
        for (ans = T{}; ~idx; idx = (idx & idx + 1) - 1)
            ans += data[idx];
        return ans;
    }

    template<typename = void>
        requires requires(T t) { { t - t } -> std::same_as<T>; }
    constexpr T query(const size_t b, const size_t e) const {
        return sum(e) - (b ? sum(b - 1) : T{});
    }
};

#endif //BINARY_INDEXED_TREE_H
