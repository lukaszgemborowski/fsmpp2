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
        return handled(); // indicate that event was handled
    }
};

TEST_CASE("Event handling by single state", "[example][fsmpp2]")
{
    fsmpp2::states<SingleState> sm;
    sm.handle(SimpleEvent1{});
    sm.handle(SimpleEvent2{});
}