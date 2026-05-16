#ifndef EXGCD_H
#define EXGCD_H
#include <concepts>

template<std::signed_integral T>
T exgcd(const T a, const T b, T &u, T &v) {
    if (!b) {
        u = 1;
        v = 0;
        return a;
    }

    const T d = exgcd(b, a % b, v, u);
    v -= a / b * u;
    return d;
}
#endif //EXGCD_H