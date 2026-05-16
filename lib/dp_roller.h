#ifndef DP_ROLLER_H
#define DP_ROLLER_H

/**
 *
 * @brief a util class that simplify tedious dp code
 * the whole class is composed of a
 */
template<typename T>
struct dp_roller {
    const size_t _size;
    std::vector<T> data;
    /// where the dp data stores, double the amount of allocated memory to lessen data move times without increase space complexity
    typename std::vector<T>::iterator reset_place; /// where the cur_idx should be reset to
    typename std::vector<T>::iterator cur_idx; /// a pointer pointing to currently active(calculating) element in data
    const T default_value;

    explicit dp_roller(const size_t n): dp_roller(n, T{}) {
    }

    explicit dp_roller(const size_t n, const T &value): _size(n), data(2 * n, value), reset_place(data.begin() + n),
                                                        cur_idx(reset_place), default_value(value) {
    }

    /**
     * @brief inc the cur_idx, roll back if overflow
     * @returns new value of cur_idx
     */
    typename std::vector<T>::iterator roll() {
        if (++cur_idx == data.end()) {
            cur_idx = reset_place;
            for (size_t i = 0; i < _size; ++i) {
                data[i] = std::move(data[i + _size]);
                data[i + _size] = default_value;
            }
        }
        return cur_idx;
    }

    /**
     * @param n times of iteration
     * @param f a function receives loop variable and returns a newly calculated value
     * @return last calculated element
     */
    template<typename F>
        requires requires(F f, size_t idx)
        {
            { f(idx) } -> std::same_as<void>;
        }
    T &routine(const size_t n, F f) {
        for (size_t i = 0; i < n; ++i) {
            f(i);
            roll();
        }

        return *(cur_idx - 1);
    }

    T &operator[](const ptrdiff_t offset) {
        assert(offset <= 0 && offset + static_cast<ptrdiff_t>(_size) >= 0);
        return *(cur_idx + offset);
    }
};

#endif //DP_ROLLER_H
