#ifndef FSMPP2_DETAIL_HPP
#define FSMPP2_DETAIL_HPP

#include "fsmpp2/meta.hpp"

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
struct NoSubStates {
    template<class E> auto handle(E const&) { return false; }
};

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_HPP