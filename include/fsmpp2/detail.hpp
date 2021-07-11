#ifndef FSMPP2_DETAIL_HPP
#define FSMPP2_DETAIL_HPP

#include "fsmpp2/meta.hpp"

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

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_HPP