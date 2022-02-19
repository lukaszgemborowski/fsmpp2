#ifndef FSMPP2_STATE_CONTAINER_HPP
#define FSMPP2_STATE_CONTAINER_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/access_context.hpp"
#include "fsmpp2/detail/traits.hpp"
#include <variant>


namespace fsmpp2::detail
{

/**
 * Wrapper around std::variant, which is current implementation
 * of states container. This is done in separate class to abstract it
 * out in case we want to replace std::variant with something else.
 **/
template<class States>
class state_container {
private:
    using type_list = typename States::type_list;

public:
    /**
     * Enter 'State' state.
     *
     * It is realized by emplacing a new State object within a std::variant.
     **/
    template<class State>
    void enter() {
        states_.template emplace<State>();
    }

    /**
     * Enter 'State' state.
     *
     * Emplace new State and pass a Context to its constructor. This is caller
     * responsibility to determine if given State have proper constructor.
     **/
    template<class State, class Context>
    void enter(Context& ctx) {
        states_.template emplace<State>(ctx);
    }

    /**
     * Exit current state.
     **/
    void exit() {
        states_.template emplace<std::monostate>();
    }

    template<class F>
    auto visit(F&& fun) {
        std::visit(std::forward<F>(fun), states_);
    }

    template<class State>
    auto is_in() const {
        return std::holds_alternative<State>(states_);
    }

    template<class State>
    auto& state() {
        return std::get<State>(states_);
    }

    template<class State, class Context>
    void init_context(Context &c) {
        if constexpr (detail::has_access_context_type<State>::value) {
            static_assert(std::is_same_v<Context, typename State::access_context_type>, "Wrong context type requested by access_context<>");
            state<State>().ctx_ = &c;
        }
    }

    template<class State, class... C>
    void init_context(contexts<C...>& c) {
        if constexpr (detail::has_access_context_type<State>::value) {
            init_accessor_ctx<State>(state<State>().ctx_, c);
        }
    }

    template<class State, class T, class... C>
    void init_accessor_ctx(T*& ptr, contexts<C...>& c) {
        // TODO check if access_context_type is in contexts<>, static_assert
        ptr = &c.template get<typename State::access_context_type>();
    }

    template<class State, class C, class... R>
    void init_accessor_ctx(std::tuple<R*...> &tup, C& c) {
        auto set_single = [&](auto& ptr) {
            ptr = &c.template get<std::remove_pointer_t<std::remove_reference_t<decltype(ptr)>>>();
        };

        (set_single(std::get<R *>(tup)), ...);
    }

private:
    // Prepend a list of states with std::monostate. We want to avoid a situation
    // that State will be constructed by defaulted when created instance of this class.
    // 1. maybe we want to defer creation of State object
    // 2. first state does not necessarily have default constructor 
    using states_variant_list = typename meta::type_list_push_front<type_list, std::monostate>::result;

    // transform a type list to a corresponding variant
    using states_variant = typename meta::type_list_rename<states_variant_list, std::variant>::result;

    states_variant states_;
};

} // namespace fsmpp2::detail

#endif // FSMPP2_STATE_CONTAINER_HPP