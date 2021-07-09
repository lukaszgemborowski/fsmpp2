#ifndef FSMPP2_STATES_HPP
#define FSMPP2_STATES_HPP

#include "fsmpp2/meta.hpp"

namespace fsmpp2
{

struct event {};

template<class... E>
using events = meta::type_list<E...>;

template<class... S>
struct states {
    using type_list = meta::type_list<S...>;
    static constexpr auto count = meta::type_list_size(type_list{});
};

template<class SubStates = states<>>
struct state {
    using substates_type = SubStates;

    template<class S>
    auto transition() const {
        return transitions<S>{0};
    }

    auto not_handled() const {
        return transitions<>{detail::not_handled{}};
    }

    auto handled() const {
        return transitions<>{detail::handled{}};
    }
};

} // namespace fsmpp2

#endif // FSMPP2_STATES_HPP