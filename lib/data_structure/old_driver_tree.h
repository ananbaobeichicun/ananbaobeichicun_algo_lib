#ifndef OLD_DRIVER_TREE_H
#define OLD_DRIVER_TREE_H
#include <concepts>
#include <utility>
#include <map>

template<typename T, typename SPLIT_RULE>
    requires requires(SPLIT_RULE split_rule, size_t l, size_t mid, size_t r, T t)
{
    { split_rule(l, mid, r, std::move(t)) } -> std::convertible_to<std::pair<T, T> >;
}
struct old_driver_tree {
    std::map<size_t, T> data;

    explicit old_driver_tree(const size_t n): data({{0, T{}}, {n, T{}}}) {
    }

    std::map<size_t, T>::iterator split(const size_t idx) {
        auto it = data.lower_bound(idx);
        if (idx == it->first)
            return it;
        const size_t r = it->first;
        --it;
        const size_t l = it->first;
        const T val = std::move(it->second);

        data.erase(it);
        const auto [lval, rval] = SPLIT_RULE{}(l, idx, r, std::move(val));
        data.emplace(l, lval);
        return data.emplace(idx, rval).first;
    }

    template<typename F, typename G>
        requires requires(F f, G g, size_t l, size_t r, T t)
        {
            f(l, r, std::move(t));
            g(l, r, t);
        }
    void perform_and_assign(const size_t l, const size_t r, T &&val, F f, G g) {
        const auto itr = split(r);
        const auto itl = split(l);
        auto itt = itl;
        ++itt;
        for (auto it = itl; it != itr; ++it, ++itt)
            f(it->first, itt->first, std::move(it->second));
        data.erase(itl, itr);
        auto res_it = data.emplace(l, std::move(val)).first;
        g(res_it->first, itr->first, res_it->second);
    }

    void assign(const size_t l, const size_t r, T &&val) {
        perform_and_assign(l, r, std::move(val), [](size_t, size_t, T &&) {
                           }, [](size_t, size_t, const T &) {
                           });
    }
};
#endif //OLD_DRIVER_TREE_H