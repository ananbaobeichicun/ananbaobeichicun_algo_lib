#ifndef UNTITLED_DISJOINT_SET_H
#define UNTITLED_DISJOINT_SET_H

#include <memory>
#include <vector>

#include "util.h"

class disjoint_set_node : public std::enable_shared_from_this<disjoint_set_node> {
    std::shared_ptr<disjoint_set_node> par;

    friend std::shared_ptr<disjoint_set_node> merge(const std::shared_ptr<disjoint_set_node> &x,
                                                    const std::shared_ptr<disjoint_set_node> &y);

public:
    disjoint_set_node() = default;

    explicit disjoint_set_node(const std::shared_ptr<disjoint_set_node> &s) : par(s->find_root()) {
    }

    disjoint_set_node(const disjoint_set_node &node) : disjoint_set_node(std::make_shared<disjoint_set_node>(node)) {
    }

    // NOLINTNEXTLINE(*-no-recursion)
    std::shared_ptr<disjoint_set_node> find_root() {
        if (!par)
            return this->shared_from_this();
        return par = par->find_root();
    }
};

bool is_in_same_set(const std::shared_ptr<disjoint_set_node> &x, const std::shared_ptr<disjoint_set_node> &y) {
    return x->find_root() == y->find_root();
}


std::shared_ptr<disjoint_set_node> merge(const std::shared_ptr<disjoint_set_node> &x,
                                         const std::shared_ptr<disjoint_set_node> &y) {
    auto rootX = x->find_root();
    auto rootY = y->find_root();

    if (!is_in_same_set(rootX, rootY))
        rootY->par = rootX; // Make one set a child of the other
    return rootX;
}

template<all_same<std::shared_ptr<disjoint_set_node> >... Rest>
std::shared_ptr<disjoint_set_node> merge(std::shared_ptr<disjoint_set_node> fst, Rest... rest) {
    (merge(fst, rest), ...); // fold expression to apply merge to the rest
    return fst;
}

class disjoint_set {
    std::vector<std::shared_ptr<disjoint_set_node> > v;

public:
    disjoint_set() = default;

    explicit disjoint_set(const size_t n): v(n) {
        for (auto& i: v)
            i = std::make_shared<disjoint_set_node>();
    }

    [[nodiscard]] bool is_in_same_set(const size_t x, const size_t y) const {
        return ::is_in_same_set(v[x], v[y]);
    }

    template<typename... Args>
        requires (std::is_same_v<Args, std::size_t> && ...)
    void merge(Args... args) {
        ::merge(v[args]...);
    }
};

#endif //UNTITLED_DISJOINT_SET_H