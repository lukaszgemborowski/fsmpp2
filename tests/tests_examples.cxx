#include "catch.hpp"
#include "fsmpp2/fsmpp2.hpp"

namespace
{

// define events, need to derive from fsmpp2::event base class
struct SimpleEvent1 : fsmpp2::event {};
struct SimpleEvent2 : fsmpp2::event {};
struct SimpleEvent3 : fsmpp2::event {
    SimpleEvent3(int v) : value {v} {}
    int value = 0;
};

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

    // this handler can either result in transition to StateB or StateC,
    // this needs to be denoted in handler return type. It needs to consist
    // of a list of all posible states the the handler can lead to
    auto handle(SimpleEvent3 const& e) -> fsmpp2::transitions<StateB, StateC> {
        if (e.value == 1) {
            return transition<StateB>();
        } else if (e.value == 2) {
            return transition<StateC>();
        } else {
            return handled();
        }
    }
};


struct StateB : fsmpp2::state<> {
    auto handle(SimpleEvent1 const &) {
        return transition<StateA>();
    }
};

struct StateC : fsmpp2::state<> {
    auto handle(SimpleEvent1 const &) {
        return transition<StateA>();
    }
};

struct Context {
    int ev1count = 0;
    int ev2count = 0;
};

struct StateWithContext : fsmpp2::state<Context>
{
    StateWithContext(Context& ctx)
        : context_ {ctx}
    {
    }

    auto handle(SimpleEvent1 const&) {
        ++ context_.ev1count;
        return handled();
    }

    auto handle(SimpleEvent2 const&) {
        ++ context_.ev2count;
        return handled();
    }

    Context& context_;
};

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

TEST_CASE("Transition choice", "[example][fsmpp2]")
{
    fsmpp2::states<StateA, StateB, StateC> sm;

    REQUIRE(sm.is_in<StateA>());

    // event 3 with value 1 should transit state machine to state B
    sm.handle(SimpleEvent3{1});
    REQUIRE(sm.is_in<StateB>());

    // get back to state A with event 1
    sm.handle(SimpleEvent1{});
    REQUIRE(sm.is_in<StateA>());

    // event 3 with value 2 should transit state machine to state C
    sm.handle(SimpleEvent3{2});
    REQUIRE(sm.is_in<StateC>());
}

TEST_CASE("Context usage", "[example][fsmpp2]")
{
    Context context;

    // each of the states in a states set needs to declare (by inheriting state<Context>)
    // the same type of the context that it wants to use. States will forward Context
    // reference to state constructor
    fsmpp2::states<StateWithContext> sm{context};

    REQUIRE(sm.is_in<StateWithContext>());
    REQUIRE(context.ev1count == 0);
    REQUIRE(context.ev2count == 0);

    sm.handle(SimpleEvent1{});
    REQUIRE(context.ev1count == 1);

    sm.handle(SimpleEvent2{});
    REQUIRE(context.ev2count == 1);
}