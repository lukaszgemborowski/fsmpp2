#include "catch.hpp"
#include "fsmpp2/reflection.hpp"
#include "fsmpp2/fsmpp2.hpp"

namespace
{

struct Context{};
struct StateName : fsmpp2::state<> {};
struct StateA : fsmpp2::state<> {};
struct StateB : fsmpp2::state<> {};

struct EventA : fsmpp2::event {};
struct EventB : fsmpp2::event {};
struct EventC : fsmpp2::event {};

struct EventHandlingState : fsmpp2::state<> {
    auto handle(EventA const&) const { return handled(); }
    auto handle(EventB const&) const { return not_handled(); }
    auto handle(EventC const&) const -> fsmpp2::transitions<StateA, StateB>;
};

}

TEST_CASE("State name", "[!nonportable][reflection][class_type_name]")
{
    auto name = fsmpp2::reflection::class_type_name<StateName>();
    REQUIRE(name == "(anonymous namespace)::StateName");
}

TEST_CASE("Event handlers return values", "[!nonportable][reflection][state_handle_result_type]")
{
    auto handled = fsmpp2::reflection::state_handle_result_type<EventHandlingState, EventA>();
    REQUIRE(handled == "fsmpp2::transitions<>");

    auto not_handled = fsmpp2::reflection::state_handle_result_type<EventHandlingState, EventB>();
    REQUIRE(not_handled == "fsmpp2::transitions<>");

    auto ab_transition = fsmpp2::reflection::state_handle_result_type<EventHandlingState, EventC>();
    REQUIRE(ab_transition == "fsmpp2::transitions<(anonymous namespace)::StateA, (anonymous namespace)::StateB>");
}

TEST_CASE("List of target states", "[!nonportable][reflection][state_handle_transition_to]")
{
    auto target = fsmpp2::reflection::state_handle_transition_to<EventHandlingState, EventC>();
    REQUIRE(std::vector<std::string>{"(anonymous namespace)::StateA", "(anonymous namespace)::StateB"} == target);
}

TEST_CASE("List all states in a set", "[!nonportable][reflection][state_names]")
{
    using SM = fsmpp2::states<StateName, StateA, StateB>;
    auto all_states = fsmpp2::reflection::state_names<SM>::get();

    REQUIRE(std::vector<std::string>{
                "(anonymous namespace)::StateName",
                "(anonymous namespace)::StateA",
                "(anonymous namespace)::StateB"} ==
                    all_states);
}

TEST_CASE("Describe flat state machine", "[!nonportable][reflection][state_machine_description]")
{
    using SM = fsmpp2::states<EventHandlingState, StateA, StateB>;
    using Events = fsmpp2::events<EventA, EventB, EventC>;
    using Description = fsmpp2::reflection::state_machine_description<SM, Events>;

    auto descr = Description::get();

    using namespace std::string_literals;

    REQUIRE(descr.size() == 3);

    CHECK(descr[0].name == "(anonymous namespace)::EventHandlingState"s);
    REQUIRE(descr[0].event_transitions.size() == 1);
        CHECK(descr[0].event_transitions[0].event == "(anonymous namespace)::EventC"s);
        REQUIRE(descr[0].event_transitions[0].states.size() == 2);
            CHECK(descr[0].event_transitions[0].states[0] == "(anonymous namespace)::StateA"s);
            CHECK(descr[0].event_transitions[0].states[1] == "(anonymous namespace)::StateB"s);

    CHECK(descr[1].name == "(anonymous namespace)::StateA"s);
    CHECK(descr[1].event_transitions.size() == 0);
    CHECK(descr[2].name == "(anonymous namespace)::StateB"s);
    CHECK(descr[2].event_transitions.size() == 0);
}