#ifndef FSMPP2_STATE_MACHINE_HPP
#define FSMPP2_STATE_MACHINE_HPP

#include "fsmpp2/detail/state_manager.hpp"
#include "fsmpp2/contexts.hpp"

namespace fsmpp2
{

template<class States, class Events, class Context, class Tracer = detail::NullTracer>
class state_machine {
public:
    using context_type = Context;
    using states_type = States;
    using events_type = Events;
    using tracer_type = Tracer;

    /**
     * Creates a state machine.
     *
     * All template parameters must by provided explicitly,
     * Context will be created internally as regular data member.
     **/
    template<
        class T = Context,
        std::enable_if_t<std::is_constructible_v<T>, bool> = true>
    state_machine()
        : context_ {}
        , manager_ {context_, tracer_}
    {
    }

    /**
     * Creates a state machine with a reference to a context.
     * 
     * This construct is to be used when user does not rely on CTAD and is providing
     * all state_machine<> template arguments explicitly.
     **/
    template<
        class T = Context,
        // enable this constructor only if Context is an lvalue ref
        std::enable_if_t<std::is_lvalue_reference_v<T>, bool> = true>
    state_machine(Context& ctx)
        : context_ {ctx}
        , manager_ {context_, tracer_}
    {
    }

    template<
        class T = Context,
        // enable this constructor only if Context is an rvalue ref
        std::enable_if_t<!std::is_lvalue_reference_v<T>, bool> = true>
    state_machine(Context&& ctx)
        : context_ {std::move(ctx)}
        , manager_ {context_, tracer_}
    {
    }

    /**
     * Creates a state machine with a context.
     *
     * When using this constructor all class template arguments can be deduced automatically.
     **/
    template<class S, class E, class C>
    state_machine(S, E, C&& ctx)
        : context_ {std::forward<C>(ctx)}
        , manager_ {context_, tracer_}
    {
    }

    /**
     * Creates a state machine with a context and a tracer.
     **/
    state_machine(States, Events, Context& ctx, Tracer&& tracer)
        : context_ {ctx}
        , tracer_ {std::move(tracer)}
        , manager_ {context_, tracer}
    {
    }

    /**
     * Dispatch an event to a current state.
     **/
    template<class E>
    auto dispatch(E const& e) {
        return manager_.dispatch(e);
    }

    /**
     * Dispatch an event to a current state.
     **/
    template<class E>
    auto dispatch(E const& e) const {
        return manager_.dispatch(e);
    }

    /**
     * Gets a reference to a tracer object.
     **/
    auto& tracer() {
        return tracer_;
    }

    /**
     * Gets a reference to a tracer object.
     **/
    auto const& tracer() const {
        return tracer_;
    }

    /**
     * Gets a reference to a context.
     **/
    auto& context() {
        return context_;
    }

    /**
     * Gets a reference to a context.
     **/
    auto const& context() const {
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
template<class S, class E, class C> state_machine(S, E, C&&) -> state_machine<S, E, C>;

} // namespace fsmpp2

#endif // FSMPP2_STATE_MACHINE_HPP