#ifndef FSMPP2_STATE_CONTAINER_HPP
#define FSMPP2_STATE_CONTAINER_HPP

#include "fsmpp2/meta.hpp"
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
    template<class State>
    void enter() {
        states_.template emplace<State>();
    }

    template<class State, class Context>
    void enter(Context& ctx) {
        states_.template emplace<State>(ctx);
    }

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

private:
    // determine type of variant<empty_states, States...>
    using states_variant_list = typename meta::type_list_push_front<type_list, std::monostate>::result;
    using states_variant = typename meta::type_list_rename<states_variant_list, std::variant>::result;

    states_variant states_;
};

} // namespace fsmpp2::detail

#endif // FSMPP2_STATE_CONTAINER_HPP