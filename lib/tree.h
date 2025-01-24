#ifndef UNTITLED_TREE_H
#define UNTITLED_TREE_H

#include <any>
#include <cassert>
#include <functional>
#include <memory>
#include <utility>

template<class T, bool enable_size>
class binary_tree;

enum class child_type {
    root, left, right
};

template<class T = size_t, std::function<void(T &, const T &)> adder, std::function<void(T &, const T &)> minuser>
class accumulative_base {
public:
    T value;

    void add(accumulative_base other) {
        adder(value, other.value);
    }

    void minus(accumulative_base other) {
        minuser(value, other.value);
    }
};

template<typename T>
using accumulative_plus = accumulative_base<T,
    [](T &v, const T &other) { v += other; },
    [](T &v, const T &other) { v -= other; }>;

// template<typename T>
// concept accumulative_base_c = std::is_same_v<T, accumulative_base<> >;

template<typename... Args>
class accumulative_group {
    std::tuple<Args...> monitors;
};

using void_accumulative_group = accumulative_group<>;

template<class T, typename AGT> //accumulative_group_type
class binary_tree<T, false> : public std::enable_shared_from_this<binary_tree<T, false> > {
public:
    using node_T = binary_tree;
    using node_ptr_T = std::shared_ptr<node_T>;
    using node_wptr_T = std::weak_ptr<node_T>;

protected:
    node_wptr_T par;
    node_ptr_T left, right;
    T value;

    //todo 支持多类型
    static constexpr std::vector<accumulative_base<std::any> node_T::*> accumulatives = {};

public:
    child_type position = child_type::root;

    binary_tree(const node_T &that) : par(that.par), left(that.left), right(that.right),
                                      value(that.value) {
        if (auto p = that.par; !p.expired())
            p.lock()->set_child(this->shared_from_this(), that.position);
        if (auto l = that.left)
            l->set_par(this->shared_from_this(), child_type::left);
        if (auto r = that.right)
            r->set_par(this->shared_from_this(), child_type::right);
    }

    binary_tree(node_T &&that) noexcept : par(that.par), left(that.left), right(that.right),
                                          value(std::move(that.value)) {
        if (auto p = that.par; !p.expired())
            p.lock()->set_child(this->shared_from_this(), that.position);
        if (auto l = that.left)
            l->set_par(this->shared_from_this(), child_type::left);
        if (auto r = that.right)
            r->set_par(this->shared_from_this(), child_type::right);
    }


    explicit binary_tree(const T &value, node_ptr_T par = nullptr, const child_type position = child_type::root,
                         node_ptr_T left = nullptr, node_ptr_T right = nullptr) : value(value) {
        set_par(par, position);
        set_children(left, right);
    }

    explicit binary_tree(T &&value = T(), node_ptr_T par = nullptr, const child_type position = child_type::root,
                         node_ptr_T left = nullptr, node_ptr_T right = nullptr) : value(std::move(value)) {
        set_par(par, position);
        set_children(left, right);
    }

    void update(node_ptr_T x, const bool is_adder) {
        if constexpr (std::is_same_v<AGT, void_accumulative_group>)
            return;
        for (auto i: this->accumulatives)
            if (is_adder)
                (this->*i).adder(*x.*i);
            else
                (this->*i).minuser(*x.*i);
        if (!this->par.expired())
            this->par.lock()->update(x, is_adder);
    }

    [[nodiscard]] node_ptr_T &get_child(const child_type t) {
        switch (t) {
            case child_type::left:
                return left;
            case child_type::right:
                return right;
            case child_type::root:
                throw std::invalid_argument("Invalid child_type");
            default:
                std::unreachable();
        }
    }

    void set_par(const node_ptr_T x, child_type t) {
        if (!this->par.expired()) {
            this->par.lock()->update(this->shared_from_this(), false);
            this->par.lock()->get_child(position) = nullptr;
        }

        if (!x) {
            assert(t == child_type::root);
            this->par.reset();
            this->position = child_type::root;
            return;
            // return this->shared_from_this();
        }
        this->par = x;
        this->position = t;
        x->update(this->shared_from_this(), true);
        x->get_child(t) = this->shared_from_this();
        // return this->shared_from_this();
    }

    void set_child(const node_ptr_T x, child_type t) {
        if (const auto child = this->get_child(t)) {
            //不知道为什么去掉.get就会误报后面的代码不能到达
            // ReSharper disable CppDFAUnreachableCode
            this->update(child, false);
            child->par.reset();
            child->position = child_type::root;
        }
        if (!x) {
            assert(t != child_type::root);
            get_child(t) = nullptr;
            return;
            // return this->shared_from_this();
        }
        this->update(x, true);
        this->get_child(t) = x;
        x->position = t;
        x->par = this->shared_from_this();
        // return this->shared_from_this();
        // ReSharper restore CppDFAUnreachableCode
    }

    void set_children(const node_ptr_T x, const node_ptr_T y) {
        this->set_child(x, child_type::left);
        this->set_child(y, child_type::right);
        // return this->shared_from_this();
    }

    void make_child(const T &v, const child_type t) {
        return this->set_child(std::make_shared<node_T>(v), t);
    }

    void make_child(T &&v, const child_type t) {
        return this->set_child(std::make_shared<node_T>(std::move(v)), t);
    }
};
#endif //UNTITLED_TREE_H
