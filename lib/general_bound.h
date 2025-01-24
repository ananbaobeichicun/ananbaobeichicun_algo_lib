#ifndef GENERAL_BOUND_H
#define GENERAL_BOUND_H
#include <functional>

#include "math.h"

template<typename T>
std::pair<T, T> boundary(T start, T end, std::function<T(T, T)> aver, std::function<bool(T)> pred) {
    T lower = start, upper = end, mid = aver(start, end);
    while (lower != mid && upper != mid) {
        if (pred(mid))
            upper = mid;
        else
            lower = mid;
        mid = aver(lower, upper);
    }
    return {lower, upper};
}

template<std::integral T>
std::pair<T, T> boundary(T start, T end, std::function<bool(T)> pred) {
    return boundary(start, end, aver<T>, pred);
}

template<typename T>
std::pair<T, T> boundary_endless(T start, std::function<T(T)> enlarge, std::function<T(T, T)> aver,
                                 std::function<bool(T)> pred) {
    T end = start;
    while (!pred(end))
        end = enlarge(end);
    return boundary(start, end, aver, pred);
}

#endif //GENERAL_BOUND_H
