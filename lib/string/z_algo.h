#ifndef UNTITLED_Z_ALGO_H
#define UNTITLED_Z_ALGO_H
#include <vector>
#include "../util.h"

static std::vector<size_t> z_algo(const std::string &s) {
    const size_t n = s.size();
    if (!n)
        return {};
    std::vector<size_t> z(n);
    size_t r = 0, l = 0;
    z[0] = n;
    for (size_t i = 1; i < n; ++i) {
        size_t len = i < r ? std::min(z[i - l], r - i) : 0;
        while (i + len < n && s[i + len] == s[len])
            ++len;
        if (cmax(r, i + len))
            l = i;
        z[i] = len;
    }

    return z;
}

#endif //UNTITLED_Z_ALGO_H
