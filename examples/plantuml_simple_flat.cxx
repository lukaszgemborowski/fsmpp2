#include <iostream>
#include "fsmpp2/plantuml.hpp"

namespace events
{

struct Ev1 : fsmpp2::event {};
struct Ev2 : fsmpp2::event {};
struct Ev3 : fsmpp2::event {};

} // namespace events

namespace states
{
struct EmptyContext {};

struct A;
struct B;
struct C;
struct D;
struct E;
struct F;

struct A : fsmpp2::state<EmptyContext>
{
    auto handle(events::Ev1 const&) -> fsmpp2::transitions<B>;
};

struct B : fsmpp2::state<EmptyContext>
{
    auto handle(events::Ev1 const&) -> fsmpp2::transitions<C>;
    auto handle(events::Ev2 const&) -> fsmpp2::transitions<A>;
};

struct C : fsmpp2::state<EmptyContext>
{
    auto handle(events::Ev1 const&) -> fsmpp2::transitions<A, D>;
    auto handle(events::Ev2 const&) -> fsmpp2::transitions<F>;
};

struct D : fsmpp2::state<EmptyContext>
{
    auto handle(events::Ev3 const&) -> fsmpp2::transitions<E>;
};

struct E : fsmpp2::state<EmptyContext>
{
    auto handle(events::Ev3 const&) -> fsmpp2::transitions<F>;
};

struct F : fsmpp2::state<EmptyContext> {};

} // namespace states

int main()
{
    using SM = fsmpp2::states<states::A, states::B, states::C, states::D, states::E, states::F>;
    using Events = fsmpp2::events<events::Ev1, events::Ev2, events::Ev3>;
    
    fsmpp2::plantuml::print_state_diagram<SM, Events>(std::cout);
}