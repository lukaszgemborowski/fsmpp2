#include "catch.hpp"
#include "fsmpp2/detail/traits.hpp"

TEST_CASE("Can handle event trait", "[traits][can_handle_event]")
{
    struct Ev1 {};
    struct Ev2 {};

    struct Handler {
        void handle(Ev1) {}
    };

    CHECK(fsmpp2::detail::can_handle_event<Handler, Ev1>::value == true);
    CHECK(fsmpp2::detail::can_handle_event<Handler, Ev2>::value == false);
}

TEST_CASE("Can handle event with context trait", "[traits][can_handle_event_with_context]")
{
    struct Ev1 {};
    struct Ev2 {};
    struct Ctx1 {};
    struct Ctx2 {};

    struct Handler {
        void handle(Ev1, Ctx1) {}
    };

    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx1>::value == true);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx1>::value == false);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx2>::value == false);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx2>::value == false);
}