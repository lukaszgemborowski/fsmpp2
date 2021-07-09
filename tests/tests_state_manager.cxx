#include "fsmpp2/fsmpp2.hpp"
#include "catch.hpp"

namespace
{

struct AContext {
    int shared_value = 0;
};

struct Ev1 : fsmpp2::event {};
struct Ev2 : fsmpp2::event {};
struct Ev3 : fsmpp2::event {};

struct StateB : fsmpp2::state<> {
    StateB(AContext &ctx)
    {
        ctx.shared_value = 42;
    }
};

struct StateA : fsmpp2::state<> {
    auto handle(Ev1 const&) {
        eventHandled = true;
        return handled();
    }

    auto handle(Ev3 const&) const {
        return transition<StateB>();
    }

    bool eventHandled = false;
};

}

TEST_CASE("State manager basic operations", "[state_manager]")
{
    AContext ctx;
    fsmpp2::state_manager<fsmpp2::states<StateA, StateB>, AContext> sm{ctx};

    REQUIRE(sm.is_in<StateA>() == true);
    REQUIRE(sm.is_in<StateB>() == false);

    SECTION("Leaving a state") {
        sm.exit();
        CHECK(sm.is_in<StateA>() == false);
        CHECK(sm.is_in<StateB>() == false);
    }

    SECTION("Handling an non-transition event") {
        CHECK(sm.dispatch(Ev1{}));
        CHECK(sm.state<StateA>().eventHandled);
        CHECK(sm.is_in<StateA>() == true);
    }

    SECTION("Dispatching non handled event") {
        CHECK(sm.dispatch(Ev2{}) == false);
        CHECK(sm.state<StateA>().eventHandled == false);
        CHECK(sm.is_in<StateA>() == true);
    }

    SECTION("State transition") {
        CHECK(ctx.shared_value == 0);
        CHECK(sm.dispatch(Ev3{}));
        CHECK(ctx.shared_value == 42);
        CHECK(sm.is_in<StateB>());
    }
}

namespace
{

struct InnerOuterCtx {
    bool innerConstructed = false;
    bool outerConstructed = false;
    bool ev1handled = false;
    bool ev2handled = false;
    int ev3count = 0;
};

struct InnerState : fsmpp2::state<>
{
    InnerOuterCtx& ctx;

    InnerState(InnerOuterCtx& ctx) : ctx{ctx}
    {
        ctx.innerConstructed = true;
    }

    auto handle(Ev1 const&) {
        ctx.ev1handled = true;
        return handled();
    }

    auto handle(Ev3 const&) {
        ctx.ev3count ++;
        return not_handled();
    }
};

struct OuterState : fsmpp2::state<fsmpp2::states<InnerState>>
{
    InnerOuterCtx& ctx;
    OuterState(InnerOuterCtx& ctx) : ctx{ctx}
    {
        ctx.outerConstructed = true;
    }

    auto handle(Ev2 const&) {
        ctx.ev2handled = true;
        return handled();
    }

    auto handle(Ev3 const&) {
        ctx.ev3count ++;
        return handled();
    }
};
}

TEST_CASE("State manager substate", "[state_manager]")
{
    InnerOuterCtx ctx;
    fsmpp2::state_manager<fsmpp2::states<OuterState>, InnerOuterCtx> sm{ctx};

    CHECK(ctx.innerConstructed);
    CHECK(ctx.outerConstructed);

    sm.dispatch(Ev1{});
    CHECK(ctx.ev1handled);

    sm.dispatch(Ev2{});
    CHECK(ctx.ev2handled);

    sm.dispatch(Ev3{});
    CHECK(ctx.ev3count == 2);
}