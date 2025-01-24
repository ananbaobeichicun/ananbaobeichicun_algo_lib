#ifndef CONTAINER_IO_H
#define CONTAINER_IO_H
#include <istream>
#include <vector>

template<typename T>
class direct_input_able_t {
public:
    constexpr static bool data = requires(T t, std::istream is) { is >> t; };
};

template<typename T>
class direct_output_able_t {
public:
    constexpr static bool data = requires(T t, std::ostream os) { os << t; };
};

template<typename T, template<typename> typename direct_io_able_t>
    requires requires
    {
        { direct_io_able_t<T>::data } -> std::same_as<const bool &>;
    }
constexpr bool io_able_v = direct_io_able_t<T>::data;

template<typename T, template<typename> typename direct_io_able_t>
    requires requires
    {
        { direct_io_able_t<T>::data } -> std::same_as<const bool &>;
    }
constexpr bool io_able_v<std::vector<T>, direct_io_able_t> = direct_io_able_t<T>::data;

template<typename T>
concept inputable = io_able_v<T, direct_input_able_t>;

template<typename T>
concept outputable = io_able_v<T, direct_output_able_t>;

template<inputable T>
std::istream &operator>>(std::istream &is, std::vector<T> &v) {
    for (T &x: v)
        is >> x;
    return is;
}

template<outputable T>
std::ostream &operator>>(std::ostream &os, const std::vector<T> &v) {
    for (const T &x: v)
        os << x;
    return os;
}
#endif //CONTAINER_IO_H
