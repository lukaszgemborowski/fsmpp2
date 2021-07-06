#ifndef FSMPP2_FSMPP2_HPP
#define FSMPP2_FSMPP2_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail.hpp"
#include "fsmpp2/context.hpp"
#include <concepts>

namespace fsmpp2
{

struct event {};

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

    transitions(detail::handled)
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    transitions(detail::not_handled)
        : idx {sizeof...(S)}
        , outcome {result::not_handled}
    {}
};

template<class Context = detail::NullContext, class SubStates = detail::NoSubStates>
struct state {
    using context_type = Context;

    template<class S>
    auto transition() const {
        return transitions<S>{0};
    }

    auto not_handled() const {
        return detail::not_handled{};
    }

    auto handled() const {
        return detail::handled{};
    }

    SubStates substates_;
};

template<class... States>
struct state_instance
{
    template<class F, class ...>
    struct state_context_type {
        using type = typename F::context_type;
    };

public:
    using context_type = typename state_context_type<States...>::type;
    using type_list = meta::type_list<States...>;

    template<class State>
    void create(context_type &ctx) {
        static_assert(meta::type_list_has<State>(type_list{}), "state is not in set");

        if constexpr (std::constructible_from<State, context_type &>) {
            new (storage_) State (ctx);
        } else {
            new (storage_) State ();
        }
        index_ = meta::type_list_index<State>(type_list{});
    }

    void destroy() {
        apply(
            [this]<typename T>(T *ptr) {
                ptr->~T();
                index_ = sizeof...(States);
            }
        );
    }

    template<class S>
    bool is_in() const {
        return type_list_index<S>(type_list{}) == index_;
    }

    template<class S>
    S& state() {
        return *reinterpret_cast<S *>(storage_);
    }

    template<class F>
    void apply(F func) {
        apply(func, std::make_index_sequence<sizeof...(States)>{});
    }

private:
    template<class F, std::size_t... Idx>
    void apply(F func, std::index_sequence<Idx...>) {
        bool executed = false;
        (apply_one<Idx>(executed, func), ...);
    }

    template<std::size_t I, class F>
    void apply_one(bool& executed, F func) {
        if (!executed && I == index_) {
            using state_type = typename meta::type_list_type<I, type_list>::type;
            func(reinterpret_cast<state_type *>(storage_));
            executed = true;
        }
    }

private:
    // TODO: extract storage type to separate class
    unsigned char storage_[detail::storage_for(meta::type_list<States...>{})];
    std::size_t index_ = sizeof...(States);
};

template<class First, class... States>
struct states
{
public:
    using type_list = meta::type_list<First, States...>;

    // TODO: verify that all states use the same context_type, otherwise static_assert
    using context_type = typename First::context_type;

    states()
        : context_ {}
        , states_ {}
    {
        states_.template create<First>(context_.value());
    }

    states(context_type &ctx)
        : context_ {ctx}
        , states_ {}
    {
        states_.template create<First>(context_.value());
    }

    ~states() {
        states_.destroy();
    }

    template<class E>
    auto handle(E const& e) {
        auto result = false;
        states_.apply(
            [this, &e, &result](auto *ptr) { result = handle(*ptr, e); }
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
    template<class S, class E>
    bool handle(S &state, E const& e) requires detail::EventHandler<S, E> {
        if (!state.substates_.handle(e)) {
            return handle_result(state.handle(e));
        } else {
            return true;
        }
    }

    template<class S, class E>
    bool handle(S& state, E const& e) {
        if (state.substates_.handle(e)) {
            return true;
        } else {
            return false;
        }
    }

    bool handle_result(detail::not_handled) {
        return false;
    }

    bool handle_result(detail::handled) {
        return true;
    }

    template<class... T>
    bool handle_result(transitions<T...> t) {
        // TODO: verify T... are in First,States...
        handle_transition(t, std::make_index_sequence<sizeof...(T)>{});
        return true; // assume true
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
    state_instance<First, States...> states_;
};

} // namespace fsmpp2

#endif // FSMPP2_FSMPP2_HPP