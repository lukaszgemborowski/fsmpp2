#include "catch.hpp"
#include "fsmpp2/fsmpp2.hpp"

namespace
{

struct Context
{
    bool innerCreated = false;
    bool outerCreated = false;
};

struct InnerState : fsmpp2::state<Context>
{
    InnerState(Context& ctx)
        : ctx_ {ctx}
    {
        ctx_.innerCreated = true;
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

    Context& ctx_;
};

}

TEST_CASE("Outer and inner state creation", "[states][state][hierarchy]")
{
    Context context;
    fsmpp2::states<OuterState> sm{context};

    REQUIRE(context.outerCreated);
    REQUIRE(context.innerCreated);
}