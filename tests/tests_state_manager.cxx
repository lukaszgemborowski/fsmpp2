#include "fsmpp2/fsmpp2.hpp"
#include "catch.hpp"

namespace
{

struct EmptyContext {};
struct Ev1 : fsmpp2::event {};
struct Ev2 : fsmpp2::event {};

struct StateA : fsmpp2::state<> {
    auto handle(Ev1 const&) {
        eventHandled = true;
        return handled();
    }

    bool eventHandled = false;
};

}

TEST_CASE("Single state manager", "[state_manager]")
{
    fsmpp2::state_manager<fsmpp2::states<StateA>, EmptyContext> sm;

    REQUIRE(sm.is_in<StateA>() == false);

    sm.enter<StateA>();
    REQUIRE(sm.is_in<StateA>() == true);

    SECTION("Leaving a state") {
        sm.exit();
        CHECK(sm.is_in<StateA>() == false);
    }

    SECTION("Handling an event") {
        CHECK(sm.dispatch(Ev1{}));
        CHECK(sm.state<StateA>().eventHandled);
    }
}