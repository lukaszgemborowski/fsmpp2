#ifndef FSMPP2_DETAIL_HPP
#define FSMPP2_DETAIL_HPP

#include "fsmpp2/meta.hpp"
#include <algorithm>
#include <array>

namespace fsmpp2::detail
{

template<class... T>
constexpr auto storage_for(meta::type_list<T...>)
{
    static_assert(sizeof...(T) > 0);

    std::array<std::size_t, sizeof...(T)> arr{sizeof(T)...};
    return *std::max_element(arr.begin(), arr.end());
}

template<class S, class E>
concept EventHandler = requires(S s, E e) {
    s.handle(e);
};

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

struct NullContext {};

template<class Q, class... S>
struct verify_same_context_type {
    template<class... X> struct check {};

    template<class X> struct check<X> {
        static constexpr auto value = std::is_same_v<Q, typename X::context_type>;
    };

    template<class X, class... T> struct check<X, T...> {
        static constexpr auto value = check<X>::value ? check<T...>::value : false;
    };

    static constexpr auto value = check<S...>::value;
};

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_HPP