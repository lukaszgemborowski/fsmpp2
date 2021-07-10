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

template<class T, class E>
class can_handle_event
{
    template<class U>
    static auto test(int) -> decltype(std::declval<U>().handle(std::declval<E>()), std::true_type{});

    template<class>
    static std::false_type test(...);

public:
    static constexpr auto value = std::is_same_v<std::true_type, decltype(test<T>(0))>;
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