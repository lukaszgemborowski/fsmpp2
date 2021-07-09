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

namespace
{
struct CtxA { bool value = false; };
struct CtxB { bool value = false; };

struct CtxStateB : fsmpp2::state<>
{
    CtxStateB(CtxB &ctx) {
        ctx.value = true;
    }
};

struct CtxStateA : fsmpp2::state<>
{
    CtxStateA(CtxA &ctx) {
        ctx.value = true;
    }

    auto handle(Ev1) { return transition<CtxStateB>(); }
};

}

TEST_CASE("Multiple contexts", "[contexts][state_machine]")
{
    CtxA ctx_a;
    CtxB ctx_b;

    fsmpp2::state_machine sm {
        fsmpp2::states<CtxStateA, CtxStateB>{},
        fsmpp2::events<Ev1>{},
        fsmpp2::contexts{ctx_a, ctx_b}
    };

    CHECK(ctx_a.value == true);
    CHECK(ctx_b.value == false);

    sm.dispatch(Ev1{});

    CHECK(ctx_a.value == true);
    CHECK(ctx_b.value == true);
}