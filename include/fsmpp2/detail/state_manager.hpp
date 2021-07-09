#ifndef FSMPP2_DETAIL_STATE_MANAGER_HPP
#define FSMPP2_DETAIL_STATE_MANAGER_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/transitions.hpp"
#include "fsmpp2/states.hpp"
#include "fsmpp2/contexts.hpp"
#include <variant>

namespace fsmpp2::detail
{

/**
 * Manages a set of state, creates, destroys and pass events to a proper state
 **/
template<class States, class Context>
struct state_manager
{
private:
    using type_list = typename States::type_list;

public:
    state_manager(Context &ctx)
        : context_ {ctx}
    {
        enter_first();
    }

    ~state_manager() {
        exit();
    }

    template<class T>
    void enter() {
        exit();

        // create substate manager
        constexpr auto Index = meta::type_list_index<T>(type_list{});
        substates_.template emplace<1 + Index>(context_);

        // construct state
        emplace_state<T>(context_);
    }

    void exit() {
        states_.template emplace<std::monostate>();
    }

    template<class E>
    auto dispatch(E const& e) {
        auto result = false;

        std::visit(
            [this, &e, &result](auto &substate) { result = substate_dispatch(substate, e); },
            substates_
        );

        if (result == false) {
            std::visit(
                [this, &e, &result](auto &state) { result = handle(state, e); },
                states_
            );
        }

        return result;
    }

    template<class S>
    bool is_in() const {
        return std::holds_alternative<S>(states_);
    }

    template<class S>
    S& state() {
        return std::get<S>(states_);
    }

private:
    template<class T, class C>
    void emplace_state(C &c) {
        if constexpr (std::is_constructible_v<T, C&>) {
            states_.template emplace<T>(c);
        } else {
            states_.template emplace<T>();
        }
    }

    template<class T, class... C>
    static constexpr bool is_constructible_by_one_of() {
        return (std::is_constructible_v<T, C&> || ...);
    }

    template<class T, class U, class... C>
    void try_emplace_state(fsmpp2::contexts<C...> &ctx) {
        if constexpr (std::is_constructible_v<T, U&>) {
            states_.template emplace<T>(ctx.template get<U>());
        }
    }

    template<class T, class... C>
    void emplace_state(fsmpp2::contexts<C...> &ctx) {
        if constexpr(is_constructible_by_one_of<T, C...>()) {
            (try_emplace_state<T, C, C...>(ctx), ...);
        } else {
            states_.template emplace<T>();
        }
    }

    void enter_first() {
        if constexpr (States::count) {
            using first_t = typename meta::type_list_first<type_list>::type;
            enter<first_t>();
        }
    }

    template<class SS, class E>
    bool substate_dispatch(SS& substate, E const &e) {
        return substate.dispatch(e);
    }

    template<class E>
    bool substate_dispatch(std::monostate, E const&) {
        return false;
    }

    template<class S, class E>
    bool handle(S &state, E const& e) requires detail::EventHandler<S, E> {
        if (handle_result(state.handle(e))) {
            return true;
        } else {
            return false;
        }
    }

    template<class S, class E>
    bool handle(S& state, E const& e) {
        return false;
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
            using transition_type_list = typename Transition::list;
            using type_at_index = typename meta::type_list_type<I, transition_type_list>::type;

            if constexpr (meta::type_list_has<type_at_index>(type_list{})) {
                enter<type_at_index>();
            }
        }
    }

private:
    // determine type of variant<empty_states, States...>
    using states_variant_list = typename meta::type_list_push_front<type_list, std::monostate>::result;
    using states_variant = typename meta::type_list_rename<states_variant_list, std::variant>::result;

    // determine type of variant<state_manager<States>...>
    template<class T> struct get_substate_manager_type {
        using type = state_manager<typename T::substates_type, Context>;
    };
    using substates_manager_list = typename meta::type_list_transform<type_list, get_substate_manager_type>::result;
    using substates_manager_list_fin = typename meta::type_list_push_front<substates_manager_list, std::monostate>::result;
    using substates_manager_variant = typename meta::type_list_rename<
        substates_manager_list_fin,
        std::variant>::result;

    Context&                    context_;
    states_variant              states_;
    substates_manager_variant   substates_;
};

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_STATE_MANAGER_HPP