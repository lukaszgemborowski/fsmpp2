#ifndef FSMPP2_DETAIL_SINGLE_STATE_INSTANCE_HPP
#define FSMPP2_DETAIL_SINGLE_STATE_INSTANCE_HPP

#include "fsmpp2/detail.hpp"
#include "fsmpp2/meta.hpp"

namespace fsmpp2::detail
{

/**
 * Single state instance, the class functionality is similar to std::variant.
 * the aim is to manage lifetime of the state reusing the same storage.
 **/
template<class... States>
struct single_state_instance
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

        new (substate_storage_) typename State::substates_type (ctx);

        if constexpr (std::is_constructible_v<State, context_type &>) {
            new (storage_) State (ctx);
        } else {
            new (storage_) State ();
        }
        index_ = meta::type_list_index<State>(type_list{});
    }

    void destroy() {
        apply(
            [this]<typename T, typename SS>(T *ptr, SS* substates) {
                ptr->~T();
                substates->~SS();
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
            using substates_type = typename state_type::substates_type;
            func(reinterpret_cast<state_type *>(storage_), reinterpret_cast<substates_type *>(substate_storage_));
            executed = true;
        }
    }

private:
    // TODO: extract storage type to separate class
    unsigned char storage_[detail::storage_for(meta::type_list<States...>{})];
    unsigned char substate_storage_[detail::storage_for(meta::type_list<typename States::substates_type...>{})];
    std::size_t index_ = sizeof...(States);
};

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_SINGLE_STATE_INSTANCE_HPP