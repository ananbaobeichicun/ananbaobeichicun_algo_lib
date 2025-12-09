#ifndef EXCRT_EQUATION_H
#define EXCRT_EQUATION_H
#include "exgcd.h"
#include "utils.h"

template<std::signed_integral T>
struct CRT_equation {
    T x, n;
};

template<std::signed_integral T>
CRT_equation<T> operator*(CRT_equation<T> eq1, CRT_equation<T> eq2) {
    if (!eq1.n || !eq2.n)
        return {0, 0};
    T u, v;
    const T d = exgcd(eq1.n, eq2.n, u, v);
    const T m = eq1.n / d * eq2.n;
    const T dx = euclid_mod(eq2.x - eq1.x, m);
    if (dx % d)
        return {0, 0};
    u = euclid_mod(u, m) * dx / d % m;
    return {(u * eq1.n + eq1.x) % m, m};
}

template<std::signed_integral T>
CRT_equation<T> &operator*=(CRT_equation<T> &eq1, CRT_equation<T> eq2) {
    return eq1 = eq1 * eq2;
}

#endif //EXCRT_EQUATION_H