#ifndef MANACHER_H
#define MANACHER_H
#include <string>
#include <vector>
#include "../util.h"

size_t manacher(const std::string &s) {
    std::string ss = "#";
    for (const char c: s) {
        ss.push_back(c);
        ss.push_back('#');
    }

    const size_t n = 2 * s.size() + 1;
    std::vector<size_t> p(n);
    size_t r = 0, c = 0, ans = 0;
    for (size_t i = 0; i < n; ++i) {
        size_t len = i < r ? std::min(p[2 * c - i], r - i) : 1;
        while (i + len < n && ~(i - len) && ss[i + len] == ss[i - len])
            ++len;
        if (cmax(r, i + len))
            c = i;
        cmax(ans, len);
        p[i] = len;
    }
    return ans - 1;
}

#endif //MANACHER_H
