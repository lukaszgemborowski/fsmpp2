#include "catch.hpp"
#include "fsmpp2/fsmpp2.hpp"

// define events, need to derive from fsmpp2::event base class
struct SimpleEvent1 : fsmpp2::event {};
struct SimpleEvent2 : fsmpp2::event {};

struct SingleState : fsmpp2::state<>
{
    SingleState() {
    }

    ~SingleState() {
    }

    // simplest event handler possible
    auto handle(SimpleEvent1 const&) {
        ++ simpleEvent1count;
        return handled(); // indicate that event was handled
    }

    int simpleEvent1count = 0;
};

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