#ifndef FSMPP_DETAIL_TRAITS_HPP
#define FSMPP_DETAIL_TRAITS_HPP

#include "fsmpp2/access_context.hpp"

namespace fsmpp2::detail
{

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

template<class T, class E, class C>
class can_handle_event_with_context
{
    static C& get_context() noexcept;

    template<class U>
    static auto test(int) -> decltype(std::declval<U>().handle(std::declval<E>(), get_context()), std::true_type{});

    template<class>
    static std::false_type test(...);

public:
    static constexpr auto value = std::is_same_v<std::true_type, decltype(test<T>(0))>;
};

} // namespace fsmpp2::detail

#endif // FSMPP_DETAIL_TRAITS_HPP