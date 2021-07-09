#include "fsmpp2/fsmpp2.hpp"
#include "catch.hpp"

namespace
{

struct EmptyContext {};
struct StateA : fsmpp2::state<> {};

}

TEST_CASE("Single state state manager", "[state_manager]")
{
    fsmpp2::state_manager<fsmpp2::states<StateA>, EmptyContext> sm;

    REQUIRE(sm.is_in<StateA>() == false);

    sm.enter<StateA>();
    REQUIRE(sm.is_in<StateA>() == true);

    sm.exit();
    REQUIRE(sm.is_in<StateA>() == false);
}