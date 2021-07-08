#ifndef FSMPP2_FSMPP2_HPP
#define FSMPP2_FSMPP2_HPP

#include "fsmpp2/detail/single_state_instance.hpp"
#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail.hpp"
#include "fsmpp2/context.hpp"
#include "fsmpp2/config.hpp"

namespace fsmpp2
{

struct event {};

template<class... E>
using events = meta::type_list<E...>;

template<class... S>
struct transitions {
    using list = meta::type_list<S...>;
    std::size_t idx;

    enum class result {
        not_handled,
        handled,
        transition
    } outcome;

    transitions(std::size_t i)
        : idx {i}
        , outcome {result::transition}
    {}

    template<class U>
    transitions(transitions<U>)
        : idx {meta::type_list_index<U>(list{})}
        , outcome {result::transition}
    {
    }

    transitions(transitions<> const& t)
        : idx {sizeof...(S)}
        , outcome {t.outcome}
    {
    }

    transitions(detail::handled)
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    transitions(detail::not_handled)
        : idx {sizeof...(S)}
        , outcome {result::not_handled}
    {}

    bool is_transition() const {
        return outcome == result::transition;
    }

    bool is_handled() const {
        return outcome == result::handled;
    }
};

template<class...> struct state_manager;

template<class Context = detail::NullContext, class SubStates = state_manager<>>
struct state {
    using context_type = Context;
    using substates_type = SubStates;

    template<class S>
    auto transition() const {
        return transitions<S>{0};
    }

    auto not_handled() const {
        return transitions<>{detail::not_handled{}};
    }

    auto handled() const {
        return transitions<>{detail::handled{}};
    }
};

template<class... States>
struct state_manager
{
public:
    using type_list = meta::type_list<States...>;
    using first_state = typename meta::type_list_first<type_list>::type;

    using context_type = typename first_state::context_type;
    static_assert(
        detail::verify_same_context_type<context_type, States...>::value,
        "All states in states set needs to have exactly the same context type, verify"
        "if you declared all states with the same context"
    );

    state_manager()
        : context_ {}
        , states_ {}
    {
        states_.template create<first_state>(context_.value());
    }

    state_manager(context_type &ctx)
        : context_ {ctx}
        , states_ {}
    {
        states_.template create<first_state>(context_.value());
    }

    ~state_manager() {
        states_.destroy();
    }

    template<class E>
    auto handle(E const& e) {
        auto result = false;
        states_.apply(
            [this, &e, &result](auto *ptr, auto *ss) { result = handle(*ptr, *ss, e); }
        );

        return result;
    }

    template<class S>
    bool is_in() const {
        return states_.template is_in<S>();
    }

    template<class S>
    S& state() {
        return states_.template state<S>();
    }

private:
    template<class S, class SubS, class E>
    bool handle(S &state, SubS &substates, E const& e) requires detail::EventHandler<S, E> {
        if (!substates.handle(e)) {
            return handle_result(state.handle(e));
        } else {
            return true;
        }
    }

    template<class S, class SubS, class E>
    bool handle(S& state, SubS &substates, E const& e) {
        if (substates.handle(e)) {
            return true;
        } else {
            return false;
        }
    }

    template<class... T>
    bool handle_result(transitions<T...> t) {
        // TODO: verify T... are in First,States...
        if (t.is_transition()) {
            handle_transition(t, std::make_index_sequence<sizeof...(T)>{});
            return true;
        }

        return t.is_handled();
    }

    template<class Transition, std::size_t... I>
    void handle_transition(Transition trans, std::index_sequence<I...>) {
        (handle_transition_impl<I>(trans), ...);
    }

    template<std::size_t I, class Transition>
    void handle_transition_impl(Transition trans) {
        if (trans.idx == I) {
            states_.destroy();

            using transition_type_list = typename Transition::list;
            using type_at_index = typename meta::type_list_type<I, transition_type_list>::type;

            if constexpr (meta::type_list_has<type_at_index>(type_list{})) {
                states_.template create<type_at_index>(context_.value());
            }
        }
    }

private:
    detail::context<context_type> context_;
    detail::single_state_instance<States...> states_;
};

template<> struct state_manager<>
{
    template<class T> state_manager(T&) {}
    template<class E> auto handle(E const&) { return false; }
};

} // namespace fsmpp2

#endif // FSMPP2_FSMPP2_HPP