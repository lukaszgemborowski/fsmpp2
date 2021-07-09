#include "catch.hpp"
#include "fsmpp2/state_machine.hpp"

namespace
{

struct Context {};

struct Ev1 : fsmpp2::event {};

struct StateB : fsmpp2::state<> {
};

struct StateA : fsmpp2::state<> {
    auto handle(Ev1 const&) const {
        return transition<StateB>();
    }
};

}

TEST_CASE("Build a state machine", "[state_machine]")
{
    using States = fsmpp2::states<StateA, StateB>;
    using Events = fsmpp2::events<Ev1>;

    fsmpp2::state_machine<States, Events, Context> sm;
    sm.dispatch(Ev1{});
}