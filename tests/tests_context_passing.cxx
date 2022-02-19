#include "catch.hpp"
#include "fsmpp2/state_machine.hpp"

namespace
{
struct Context {
    Context() = default;

    // copy, move and assigns are deleted to ensure that only reference can be passed to state, not a copy
    Context(Context &&) = delete;
    Context(Context const&) = delete;
    Context& operator=(Context &&) = delete;
    Context& operator=(Context const&) = delete;

    int some_value = 0;
};

struct AnotherContext {
    AnotherContext() = default;

    AnotherContext(AnotherContext &&) = delete;
    AnotherContext(AnotherContext const&) = delete;
    AnotherContext& operator=(AnotherContext &&) = delete;
    AnotherContext& operator=(AnotherContext const&) = delete;

    int another_value = 0;
};

struct StateNoContext : fsmpp2::state<> {
};

struct AcceptingContext : fsmpp2::state<> {
    AcceptingContext(Context &ctx) 
        : ctx_ {ctx} 
    {
        // on construction set a value to 42
        ctx_.some_value = 42;
    }

    ~AcceptingContext()
    {
        // reset to 1 on destruction
        ctx_.some_value = 1;
    }

    Context& ctx_;
};

}

TEST_CASE("Is able to construct a state not accepting defined context", "[state_machine][context]")
{
    Context ctx;
    fsmpp2::state_machine sm {fsmpp2::states<StateNoContext>{}, fsmpp2::events<>{}, ctx};
}

TEST_CASE("Is able to accept reference to external context", "[state_machine][context]")
{
    Context ctx;
    REQUIRE(ctx.some_value == 0);

    {
        // construct the state machine, pass a reference to ctx
        fsmpp2::state_machine sm {fsmpp2::states<AcceptingContext>{}, fsmpp2::events<>{}, ctx};

        // initial state should be created setting some_value to 42 at construction
        CHECK(ctx.some_value == 42);
    }

    // scope exited, state machine destroyed, state destroyed.
    CHECK(ctx.some_value == 1);
}

TEST_CASE("Is able to accept a reference to one of the contexts from a set", "[state_machine][context][contexts]")
{
    Context ctx;
    AnotherContext actx;

    REQUIRE(ctx.some_value == 0);
    REQUIRE(actx.another_value == 0);

    {
        // construct the state machine, pass a reference to ctx
        fsmpp2::state_machine sm {fsmpp2::states<AcceptingContext>{}, fsmpp2::events<>{}, fsmpp2::contexts{actx, ctx}};

        // now both Context and AnotherContext should be passed as references but only Context should
        // be accessed because AcceptingContext state is accepting only Context reference
        CHECK(ctx.some_value == 42);
        REQUIRE(actx.another_value == 0);
    }

    CHECK(ctx.some_value == 1);
}

TEST_CASE("Check single argument constructor", "[state_machine][context]")
{
    using sm_type = fsmpp2::state_machine<
            fsmpp2::states<StateNoContext>,
            fsmpp2::events<>,
            int>;

    CHECK(std::is_constructible_v<sm_type>);        // sm_type sm{};
    CHECK(std::is_constructible_v<sm_type, int&&>); // int x; sm_type sm{std::move(x)};
    CHECK(!std::is_constructible_v<sm_type, int&>); // int x; sm_type sm{x};

    using sm_type_ref = fsmpp2::state_machine<
        fsmpp2::states<StateNoContext>,
        fsmpp2::events<>,
        int &>;

    CHECK(!std::is_constructible_v<sm_type_ref>);        // sm_type sm{};
    CHECK(!std::is_constructible_v<sm_type_ref, int&&>); // int x; sm_type sm{std::move(x)};
    CHECK(std::is_constructible_v<sm_type_ref, int&>);   // int x; sm_type sm{x};
}

TEST_CASE("Check auto-deduced context type", "[state_machine][context]")
{
    SECTION("Passing lvalue reference")
    {
        int ctx = 0;
        fsmpp2::state_machine sm {fsmpp2::states<StateNoContext>{}, fsmpp2::events<>{}, ctx};
        CHECK(std::is_same_v<typename decltype(sm)::context_type, int &>);
    }

    SECTION("Passing a temporary")
    {
        fsmpp2::state_machine sm {fsmpp2::states<StateNoContext>{}, fsmpp2::events<>{}, 42};
        CHECK(std::is_same_v<typename decltype(sm)::context_type, int>);
    }

    SECTION("Moving in a context")
    {
        int ctx = 42;
        fsmpp2::state_machine sm {fsmpp2::states<StateNoContext>{}, fsmpp2::events<>{}, std::move(ctx)};
        CHECK(std::is_same_v<typename decltype(sm)::context_type, int>);
    }
}
