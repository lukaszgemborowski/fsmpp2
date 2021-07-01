#include <tuple>
#include <functional>
#include <variant>
#include <iostream>

// framework
namespace sm
{

namespace handle_outcomes
{

template<template<typename> typename To>
struct transit_to {
    template<class B>
    using dest_t = To<B>;
};

template<class>
struct is_transit_to : std::false_type {};

template<template<typename> typename T>
struct is_transit_to<transit_to<T>> : std::true_type {};

template<class T>
static constexpr auto is_transit_to_v = is_transit_to<T>::value;

struct handled {};
struct not_handled {};

} // namespace handle_outcome

template<class T>
struct handle_result {
    using result_t = T;
};

template<template <typename> typename To>
auto transition() {
    return handle_result<handle_outcomes::transit_to<To>>{};
}

inline auto handled() {
    return handle_result<handle_outcomes::handled>{};
}

inline auto not_handled() {
    return handle_result<handle_outcomes::not_handled>{};
}

struct event {
    virtual ~event() noexcept = default;
};

template<class T>
concept Event = std::is_base_of_v<event, T>;

template<class Base, class SubStates = void>
struct state {
    state(Base &)
    {}

    // dummy, to "handle" not handled events from the derived state
    template<Event E> auto handle(E const&) { return not_handled(); }
};

struct Context {};

template<
    template<typename> typename First,
    template<typename> typename... S>
struct state_set {
    state_set()
        : context_ {}
        , current_state_ {std::in_place_type<First<Context>>, context_}
    {}

    template<Event E>
    void dispatch(E const& e)
    {
        // call current states handle method
        std::visit(
            [&e, this](auto &s) { 
                auto call_handle = [&s, &e]() { return s.handle(e); };

                if constexpr (std::is_same_v<decltype(call_handle()), void>) {
                    call_handle();
                } else {
                    auto r = call_handle();
                    using return_t = decltype(r)::result_t;
                    if constexpr (handle_outcomes::is_transit_to_v<return_t>) {
                        transition<typename return_t::dest_t<Context>>();
                    }
                }
            },
            current_state_
        );
    }

    template<class Dest>
    void transition()
    {
        // clear current state
        current_state_.template emplace<state<Context>>(context_);

        // replace with a new state
        current_state_.template emplace<Dest>(context_);
    }

private:
    Context context_;
    std::variant<state<Context>, First<Context>, S<Context>...> current_state_;
};

template<class S>
struct state_machine {
    template<Event E>
    void dispatch(E const& e) {
        states_.dispatch(e);
    }

    S states_;
};

}
// usage
struct MeasureEvent : sm::event {};
struct ButtonEvent : sm::event {
    bool isPressed = true;
};

template<class Context> struct power_off;
template<class Context> struct blink_green;
template<class Context> struct blink_red;

template<class Context> struct blink_green : sm::state<Context>
{
    blink_green(Context &c) : sm::state<Context>{c}
    {}
};

template<class Context> struct blink_red : sm::state<Context>
{
    blink_red(Context &c) : sm::state<Context>{c}
    {}
};

template<class Base>
struct power_on : sm::state<Base>
{
    power_on(Base &b)
        : sm::state<Base>{b}
    {
        std::cout << "power_on enter\n";
    }

    auto handle(ButtonEvent const&) const
    {
        std::cout << "power_on ButtonEvent\n";
        return sm::transition<power_off>();
    }
};

template<class Context>
struct power_off : sm::state<Context>
{
    power_off(Context &base)
        : sm::state<Context>{base}
    {
        std::cout << "power_off enter\n";
    }

    ~power_off() {
        std::cout << "power_off exit\n";
    }

    auto handle(ButtonEvent const& e)
    {
        std::cout << "power_off ButtonEvent\n";
        return sm::transition<power_on>();
    }
};


int main()
{
    // using power_on_states = sm::substate_set<power_on, blink_green, blink_red>;
    using all_states = sm::state_set<power_off, power_on>;

    sm::state_machine<all_states> sm;
    sm.dispatch(ButtonEvent{});
    sm.dispatch(ButtonEvent{});
}
