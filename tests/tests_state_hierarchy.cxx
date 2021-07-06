#include "catch.hpp"
#include "fsmpp2/fsmpp2.hpp"

namespace
{

struct InnerEvent : fsmpp2::event {};
struct InnerEventIgnore : fsmpp2::event {};
struct OuterEvent : fsmpp2::event {};

struct Context
{
    bool innerCreated = false;
    bool innerEventHandled = false;
    bool outerCreated = false;
    bool outerEventHandled = false;
};

struct InnerState : fsmpp2::state<Context>
{
    InnerState(Context& ctx)
        : ctx_ {ctx}
    {
        ctx_.innerCreated = true;
    }

    auto handle(InnerEvent const&) const {
        ctx_.innerEventHandled = true;
        return handled();
    }

    auto handle(InnerEventIgnore const&) const {
        return not_handled();
    }

    Context& ctx_;
};

struct OuterState : fsmpp2::state<Context, fsmpp2::states<InnerState>>
{
    OuterState(Context& ctx)
        : ctx_ {ctx}
    {
        ctx_.outerCreated = true;
    }

    auto handle(OuterEvent const&) const {
        ctx_.outerEventHandled = true;
        return handled();
    }

    auto handle(InnerEventIgnore const&) const {
        ctx_.outerEventHandled = true;
        return handled();
    }

    Context& ctx_;
};

}

TEST_CASE("Outer and inner state creation and event handling", "[states][state][hierarchy]")
{
    Context context;
    fsmpp2::states<OuterState> sm{context};

    REQUIRE(context.outerCreated);
    REQUIRE(context.innerCreated);
    REQUIRE(context.innerEventHandled == false);
    REQUIRE(context.outerEventHandled == false);

    SECTION("Handle event in outer state") {
        // InnerState does not have OuterEvent handled, therefore event is passed to parent state
        sm.handle(OuterEvent{});
        REQUIRE(context.innerEventHandled == false);
        REQUIRE(context.outerEventHandled == true);
    }

    SECTION("Handle event in inner state") {
        // InnerState have handler for InnerEvent, it consumes the event by call to handle()
        sm.handle(InnerEvent{});
        REQUIRE(context.innerEventHandled == true);
        REQUIRE(context.outerEventHandled == false);
    }

    SECTION("Inner state explicitly call not_handled()") {
        // InnerState have InnerEventIgnore handler but it ignores it by calling not_handled(),
        // event should be passed to its parent state
        sm.handle(InnerEventIgnore{});
        REQUIRE(context.innerEventHandled == false);
        REQUIRE(context.outerEventHandled == true);
    }
}
