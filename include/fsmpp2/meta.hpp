#ifndef FSMPP2_META_HPP
#define FSMPP2_META_HPP

// std::size_t
#include <cstddef>

// std::is_same_v
#include <type_traits>

namespace fsmpp2::meta
{

template<class... T> struct type_list {};

template<class X, class F, class ... T>
constexpr auto type_list_index(type_list<F, T...>, std::size_t idx = 0)
{
    if (std::is_same_v<F, X>) {
        return idx;
    } else {
        if constexpr (sizeof...(T) > 0) {
            return type_list_index<X>(type_list<T...>{}, idx + 1);
        } else {
            return idx + 1;
        }
    }
}

template<std::size_t I, std::size_t Target, class F, class... T>
struct type_list_type_impl {
    using type = typename type_list_type_impl<I + 1, Target, T...>::type;
};

template<std::size_t I, class F, class... T>
struct type_list_type_impl<I, I, F, T...> {
    using type = F;
};

template<std::size_t I, class F, class... T>
struct type_list_type {};

template<std::size_t I, class F, class... T>
struct type_list_type<I, type_list<F, T...>> {
    using type = typename type_list_type_impl<0, I, F, T...>::type;
};


template<class... F>
constexpr auto type_list_size(type_list<F...>)
{
    return sizeof...(F);
}

template<class X, class F, class ... T>
constexpr bool type_list_has(type_list<F, T...> l)
{
    auto idx = type_list_index<X>(l);
    return idx < type_list_size(l);
}


} // namespace fsmpp2::meta

#endif // FSMPP2_META_HPP