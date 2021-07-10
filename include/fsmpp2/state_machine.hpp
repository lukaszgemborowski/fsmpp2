#ifndef FSMPP2_STATE_MACHINE_HPP
#define FSMPP2_STATE_MACHINE_HPP

#include "fsmpp2/detail/state_manager.hpp"
#include "fsmpp2/contexts.hpp"

namespace fsmpp2
{

template<class States, class Events, class Context, class Tracer = detail::NullTracer>
class state_machine {
public:
    state_machine()
        : context_ {}
        , manager_ {context_, tracer_}
    {
    }

    state_machine(Context& ctx)
        : context_ {ctx}
        , manager_ {context_, tracer_}
    {
    }

    template<class S, class E, class C>
    state_machine(S, E, C& ctx)
        : context_ {ctx}
        , manager_ {context_, tracer_}
    {
    }

    state_machine(States, Events)
        : context_ {}
        , manager_ {context_, tracer_}
    {
    }

    state_machine(States, Events, Context&& ctx)
        : context_ {ctx}
        , manager_ {context_, tracer_}
    {
    }

    state_machine(States, Events, Context& ctx, Tracer&& tracer)
        : context_ {ctx}
        , tracer_ {std::move(tracer)}
        , manager_ {context_, tracer}
    {
    }

    template<class E>
    auto dispatch(E const& e) {
        return manager_.dispatch(e);
    }

    auto& tracer() {
        return tracer_;
    }

    auto& context() {
        return context_;
    }

private:
    Context                                 context_;
    Tracer                                  tracer_;
    detail::state_manager<
        States,
        std::remove_reference_t<Context>,
        Tracer>                             manager_;
};

template<class S, class E, class C> state_machine(S, E, C&) -> state_machine<S, E, C&>;

} // namespace fsmpp2

#endif // FSMPP2_STATE_MACHINE_HPP