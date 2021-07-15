#ifndef FSMPP2_DETAIL_SUBSTATE_MANAGER_CONTAINER_HPP
#define FSMPP2_DETAIL_SUBSTATE_MANAGER_CONTAINER_HPP

#include "fsmpp2/meta.hpp"
#include <variant>

namespace fsmpp2::detail
{

/**
 * Similarly to state_container this class is abstracting out the
 * real storage mechanism for substates state_manager's.
 **/
template<class States, template<typename> typename Manager>
class substate_manager_container {
public:
    /**
     * Create a new substate manager for a given state.
     *
     * Forward all arguments to its constructor.
     **/
    template<class State, class... Args>
    void create(Args&... args) {
        constexpr auto Index = meta::type_list_index<State>(type_list{});
        managers_.template emplace<1 + Index>(args...);
    }

    /**
     * Visit current state_manager with a given visitor.
     **/
    template<class Fun>
    void visit(Fun&& fun) {
        std::visit(std::forward<Fun>(fun), managers_);
    }

private:
    // a list of states: type_list<A, B, C...>;
    using type_list = typename States::type_list;

    // meta-function extracting substate_type from a state
    // substate_type is a states<SubA1, SubA2, SubA3...> specialization
    template<class T> struct get_substate_manager_type {
        using type = Manager<typename T::substates_type>;
    };

    // use this meta-function to transform a list of states to a list of its substate managers in form of:
    // type_list<state_manager<states<SubA1, SubA2, SubA3...>>, state_manager<states<SubB1, SubB2, SubB3...>>...>
    using substates_manager_list = typename meta::type_list_transform<type_list, get_substate_manager_type>::result;

    // prepend with monostate for default construction
    using substates_manager_list_fin = typename meta::type_list_push_front<substates_manager_list, std::monostate>::result;

    // rename type_list<...> to std::variant<...> which will be the final storage type
    using substates_manager_variant = typename meta::type_list_rename<
        substates_manager_list_fin,
        std::variant>::result;

    substates_manager_variant managers_;
};

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_SUBSTATE_MANAGER_CONTAINER_HPP