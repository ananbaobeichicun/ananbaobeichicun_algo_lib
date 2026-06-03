#ifndef MANACHER_H
#define MANACHER_H
#include <string>
#include <vector>
#include <algorithm>
#include "../util.h"

struct manacher_result {
    size_t n = 0;
    std::vector<size_t> p, ll, rr;

    manacher_result() = default;

    explicit manacher_result(const size_t n) : n(n), p(n), ll(n), rr(n) {
    }

    [[nodiscard]] size_t get_max_p() const {
        return std::ranges::max(p);
    }

    template<typename Self>
    Self &&desaturate(this Self &&self) {
        for (size_t i = 2; i < self.n; ++i)
            if (self.ll[i - 2] >= 2)
                cmax(self.ll[i], self.ll[i - 2] - 2);
        for (size_t i = self.n - 3; ~i; --i)
            if (self.rr[i + 2] >= 2)
                cmax(self.rr[i], self.rr[i + 2] - 2);
        return std::forward<Self>(self);
    }
};

manacher_result manacher(const std::string &s) {
    std::string ss = "#";
    for (const char c: s) {
        ss.push_back(c);
        ss.push_back('#');
    }

    const size_t n = 2 * s.size() + 1;
    manacher_result res(n);
    size_t r = 0, c = 0;
    for (size_t i = 0; i < n; ++i) {
        size_t len = i < r ? std::min(res.p[2 * c - i], r - i) : 1;
        while (i + len < n && ~(i - len) && ss[i + len] == ss[i - len])
            ++len;
        if (cmax(r, i + len))
            c = i;
        cmax(res.ll[i - len + 1], len - 1);
        cmax(res.rr[i + len - 1], len - 1);
        res.p[i] = len;
    }

    return res;
}

#endif //MANACHER_H
