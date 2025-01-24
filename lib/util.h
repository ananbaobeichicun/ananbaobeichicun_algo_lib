// #ifndef UTIL_H
// #define UTIL_H
//
// // #include<memory>
//
// // 定义一个类型萃取来检测是否为 std::shared_ptr
// // template<typename T>
// // struct is_shared_ptr : std::false_type {
// // };
// //
// // template<typename T>
// // struct is_shared_ptr<std::shared_ptr<T> > : std::true_type {
// // };
// //
// // // 定义一个 concept 来使用 is_shared_ptr
// // template<typename T>
// // concept is_shared_ptr_v = is_shared_ptr<T>::value;
//
// template<typename first, typename... rest>
// concept all_same = (std::is_same_v<first, rest> && ...);
//
// template<template<typename... > typename TEMPLATE>
// template<typename T>
// template<typename... args>
// concept is_instance_of = false;
//
// template<template<typename... > typename TEMPLATE>
// template<typename T>
// template<typename... args>
// concept is_instance_of<TEMPLATE<args...>> = true;
//
// // template <template <typename...> class TEM, typename T>
// // concept IsInstanceFrom = requires {
// //     typename T::template type_param<typename T::type_param_arg>;
// //     std::is_base_of_v<TEM<typename T::type_param, typename T::type_param_arg>, T>;
// // };
// template <template <typename...> class TEM, typename T>
// concept IsInstanceFrom = std::is_base_of_v<TEM<typename T::type_param>, T>;
//
// template <typename T, typename U>
// struct TEM {};
//
// template <typename T>
// struct A {};
//
// struct B : public TEM<int, double> {};
//
// struct C {};
//
//
//     static_assert(!IsInstanceFrom<TEM, A<int>>); // false
//     static_assert(IsInstanceFrom<TEM, B>); // true
//     IsInstanceFrom<TEM, C> << std::endl; // false
// #endif //UTIL_H
