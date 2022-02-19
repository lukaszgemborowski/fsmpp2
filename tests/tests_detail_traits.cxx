#include "catch.hpp"
#include "fsmpp2/detail/traits.hpp"
#include "fsmpp2/contexts.hpp"

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
        void handle(Ev1, Ctx1 &) {}
    };

    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx1>::value == true);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx1>::value == false);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx2>::value == false);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx2>::value == false);
}


TEST_CASE("Detecting handler accepting access_context parameter", "[traits][can_handle_event_with_context][access_context]")
{
    struct Ev1 {};
    struct Ev2 {};
    struct Ev3 {};
    struct Ev4 {};
    struct Ctx1 {};
    struct Ctx2 {};
    struct Handler {
        void handle(Ev1, fsmpp2::access_context<Ctx1>) {}
        void handle(Ev2, fsmpp2::access_context<Ctx2>) {}
        void handle(Ev3, fsmpp2::access_context<Ctx1, Ctx2>) {}
        void handle(Ev4, fsmpp2::access_context<Ctx2>) {}
    };

    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx2>::value == false);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx1>::value == false);

    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev1, Ctx1>::value == true);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev2, Ctx2>::value == true);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev3, fsmpp2::contexts<Ctx1, Ctx2>>::value == true);
    CHECK(fsmpp2::detail::can_handle_event_with_context<Handler, Ev4, fsmpp2::contexts<Ctx1, Ctx2>>::value == true);
}