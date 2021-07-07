#ifndef FSMPP2_TYPE_NAMES_HPP
#define FSMPP2_TYPE_NAMES_HPP

#if defined(__GNUC__)
# if defined(__clang__)
#  error "clang is not yet supported"
# endif

#include "fsmpp2/fsmpp2.hpp"
#include <cxxabi.h>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

namespace fsmpp2::reflection
{

template<class StateType>
auto class_type_name()
{
    auto s = 0;
    return std::string{abi::__cxa_demangle(typeid(StateType).name(), 0, 0, &s)};
}

template<class StateType, class Event>
auto state_handle_result_type()
{
    if constexpr (fsmpp2::detail::EventHandler<StateType, Event>) {
        auto s = 0;
        return std::string{abi::__cxa_demangle(
            typeid(decltype(std::declval<StateType>().handle(std::declval<Event>()))).name(),
            0, 0, &s)};
    } else {
        return std::string{"fsmpp2::transitions<>"};
    }
}

template<class StateType, class Event>
auto state_handle_transition_to()
{
    auto const transitions_list_raw = state_handle_result_type<StateType, Event>();
    auto const prefix = std::string{"fsmpp2::transitions<"};
    auto const cleared = transitions_list_raw.substr(prefix.length(), transitions_list_raw.length() - prefix.length() - 1);
    std::vector<std::string> targets;
    std::stringstream ss{cleared};

    while (ss.good()) {
        std::string part;
        std::getline(ss, part, ',');

        if (part.size()) {
            if (part[0] == ' ') {
                targets.push_back(part.substr(1));
            } else {
                targets.push_back(part);
            }
        }
    }

    return targets;
}

// couldn't partially specialize a function, workaround by using a struct template
template<class StatesSet> struct state_names;
template<class... States> struct state_names<fsmpp2::states<States...>>
{
    static auto get() {
        return get<States...>();
    }

private:
    template<class... T>
    static auto get() {
        std::vector<std::string> result;
        (get_single<T>(result), ...);
        return result;
    }

    template<class T>
    static auto get_single(std::vector<std::string> &result) 
    {
        result.push_back(class_type_name<T>());
    }
};

struct state_description
{
    struct transition {
        std::string event;
        std::vector<std::string> states;
    };

    std::string name;
    std::vector<transition> event_transitions;
    std::vector<state_description> substates;
};

template<class StateMachine, class EventList>
struct state_machine_description;

template<class... States, class... Events>
struct state_machine_description<fsmpp2::states<States...>, fsmpp2::meta::type_list<Events...>>
{
    static auto get() {
        std::vector<state_description> result;
        fill_states<States...>(result);
        return result;
    };

private:
    template<class... T>
    static void fill_states(std::vector<state_description> &res) {
        (fill_single_state<T>(res), ...);
    }

    template<class T>
    static void fill_single_state(std::vector<state_description> &res) {
        state_description desc;
        desc.name = class_type_name<T>();
        fill_events_information<T, Events...>(desc);
        res.push_back(desc);
    }

    template<class S, class... E>
    static void fill_events_information(state_description &desc) {
        (fill_single_event_information<S, E>(desc), ...);
    }

    template<class S, class E>
    static void fill_single_event_information(state_description &desc) {
        state_description::transition t;
        t.event = class_type_name<E>();
        t.states = state_handle_transition_to<S, E>();

        if (t.states.size() > 0)
            desc.event_transitions.push_back(t);
    }
};

} // namespace fsmpp2::reflection

#endif // defined(__GNUC__)
#endif // FSMPP2_TYPE_NAMES_HPP