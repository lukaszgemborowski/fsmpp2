#ifndef FSMPP2_STATE_MACHINE_HPP
#define FSMPP2_STATE_MACHINE_HPP

#include "fsmpp2/detail/state_manager.hpp"

namespace fsmpp2
{

template<class States, class Events, class Context>
class state_machine {
public:
    state_machine()
        : context_ {}
        , manager_ {context_}
    {
    }

    state_machine(States, Events, Context&& ctx)
        : context_ {ctx}
        , manager_ {context_}
    {
    }

    template<class E>
    auto dispatch(E const& e) {
        return manager_.dispatch(e);
    }

private:
    Context                                context_;
    detail::state_manager<
        States,
        std::remove_reference_t<Context>>  manager_;
};

template<class States, class Events, class Context>
state_machine(States, Events, Context &) -> state_machine<States, Events, Context &>;

} // namespace fsmpp2

#endif // FSMPP2_STATE_MACHINE_HPP