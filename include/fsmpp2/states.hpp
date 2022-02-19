#ifndef FSMPP2_STATES_HPP
#define FSMPP2_STATES_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail/handle_result.hpp"
#include "fsmpp2/transitions.hpp"
#include "fsmpp2/config.hpp"
#include <tuple>

namespace fsmpp2
{

/**
 * Base class for an event.
 **/
struct event {};

#ifdef FSMPP2_USE_CPP20
template<class T>
concept Event = std::is_base_of_v<event, T>;
#endif

/**
 * Defines a set of events. This is used as a parameter to a state_machine.
 **/
#ifdef FSMPP2_USE_CPP20
    template<Event... E>
#else
    template<class... E>
#endif
using events = meta::type_list<E...>;

#ifdef FSMPP2_USE_CPP20
template<class T>
concept State = requires { typename T::state_tag; };
#endif

/**
 * Defines a set of states. This is used as a parameter to a state_machine.
 **/
#ifdef FSMPP2_USE_CPP20
template<State... S>
#else
template<class... S>
#endif
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
#ifdef FSMPP2_USE_CPP20
template<State... SubStates>
#else
template<class... SubStates>
#endif
struct state {
    using substates_type = states<SubStates...>;
    using state_tag = void;

    /**
     * Execute transition to another state.
     *
     * It is intended to be returned from state's event handler.
     **/
    template<class S>
    auto transition() const {
        return transitions<S>{detail::transition<S>{}};
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

#ifdef FSMPP2_USE_CPP20
namespace detail {
template<class...> struct is_states_list : std::false_type {};
template<class... X> struct is_states_list<states<X...>> : std::true_type {};

template<class...> struct is_events_list : std::false_type {};
template<class... X> struct is_events_list<events<X...>> : std::true_type {};
}

template<class L> concept StatesList = detail::is_states_list<L>::value;
template<class L> concept EventsList = detail::is_events_list<L>::value;
#endif

} // namespace fsmpp2

#endif // FSMPP2_STATES_HPP