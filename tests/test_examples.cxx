#include "catch.hpp"
#include "fsmpp2/fsmpp2.hpp"

namespace
{

// define events, need to derive from fsmpp2::event base class
struct SimpleEvent1 : fsmpp2::event {};
struct SimpleEvent2 : fsmpp2::event {};

struct SingleState : fsmpp2::state<>
{
    // simplest event handler possible
    auto handle(SimpleEvent1 const&) {
        ++ simpleEvent1count;
        return handled(); // indicate that event was handled
    }

    int simpleEvent1count = 0;
};

struct StateA;
struct StateB;
struct StateC;

struct StateA : fsmpp2::state<>
{
    auto handle(SimpleEvent1 const &) {
        return transition<StateB>();
    }
};

struct StateB : fsmpp2::state<> {};

}

TEST_CASE("Event handling by single state", "[example][fsmpp2]")
{
    fsmpp2::states<SingleState> sm;

    // states transit to a first state from the list, here SingleState
    REQUIRE(sm.is_in<SingleState>());

    // doesn't receive any events of type SimpleEvent1
    // checking is_in() before is essential as calling state<>
    // if not in that state results in undefined behavior
    REQUIRE(sm.state<SingleState>().simpleEvent1count == 0);

    // generate an event and verify event received count
    sm.handle(SimpleEvent1{});
    REQUIRE(sm.state<SingleState>().simpleEvent1count == 1);

    // this will be silently ignored as current state does not handle SimpleEvent2
    sm.handle(SimpleEvent2{});
}

TEST_CASE("Simple transitions between two states", "[example][fsmpp2]")
{
    fsmpp2::states<StateA, StateB> sm;

    // starting in first state from the states list
    REQUIRE(sm.is_in<StateA>());

    // SimpleEvent2 is ignored by StateA
    sm.handle(SimpleEvent2{});
    // Should be still in StateA
    REQUIRE(sm.is_in<StateA>());

    // trigger transition to StateB using SimpleEvent1
    sm.handle(SimpleEvent1{});
    REQUIRE(sm.is_in<StateB>());
}
