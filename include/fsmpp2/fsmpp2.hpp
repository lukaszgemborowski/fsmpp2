#ifndef FSMPP2_FSMPP2_HPP
#define FSMPP2_FSMPP2_HPP

#include "fsmpp2/detail/state_manager.hpp"
#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail.hpp"
#include "fsmpp2/context.hpp"
#include "fsmpp2/config.hpp"

namespace fsmpp2
{

struct event {};

template<class... E>
using events = meta::type_list<E...>;

template<class Context = detail::NullContext, class SubStates = states<>>
struct state {
    using context_type = Context;
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

#endif // FSMPP2_FSMPP2_HPP