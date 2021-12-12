#ifndef FSMPP2_STATES_HPP
#define FSMPP2_STATES_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/transitions.hpp"

namespace fsmpp2
{

/**
 * Base class for an event.
 **/
struct event {};

/**
 * Defines a set of events. This is used as a parameter to a state_machine.
 **/
template<class... E>
using events = meta::type_list<E...>;

/**
 * Defines a set of states. This is used as a parameter to a state_machine.
 **/
template<class... S>
struct states {
    using type_list = meta::type_list<S...>;
    static constexpr auto count = meta::type_list_size(type_list{});
};

/**
 * Denotes a state.
 *
 * This is basic bulding block for a state machine. Each state must derive from this
 * template to provide necessery definitions and helper functions.
 *
 * If one or more SubStates is provided state machine will create a sub state machine.
 * All events will be passed down to a sub state machine first.
 **/
template<class... SubStates>
struct state {
    using substates_type = states<SubStates...>;

    /**
     * Execute transition to another state.
     *
     * It is intended to be returned from state's event handler.
     **/
    template<class S>
    auto transition() const {
        return transitions<S>{};
    }

    /**
     * Indicate that event was not handled.
     *
     * If there's parent state machine the vent will be processed there.
     * It is intended to be returned from state's event handler.
     **/
    auto not_handled() const {
        return transitions<>{detail::not_handled{}};
    }

    /**
     * Indicate that event was handled.
     *
     * Event processing will stop.
     * It is intended to be returned from state's event handler.
     **/
    auto handled() const {
        return transitions<>{detail::handled{}};
    }
};

} // namespace fsmpp2

#endif // FSMPP2_STATES_HPP