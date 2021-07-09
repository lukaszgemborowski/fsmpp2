#ifndef FSMPP2_STATE_MACHINE_HPP
#define FSMPP2_STATE_MACHINE_HPP

#include "fsmpp2/detail/state_manager.hpp"
#include "fsmpp2/context.hpp"

namespace fsmpp2
{

template<class States, class Events, class Context>
class state_machine {
public:
    state_machine()
        : context_ {}
        , manager_ {context_.value()}
    {
    }

    state_machine(Context& ctx)
        : context_ {ctx}
        , manager_ {context_.value()}
    {
    }

    template<class E>
    auto dispatch(E const& e) {
        return manager_.dispatch(e);
    }

private:
    detail::context<Context>                context_;
    detail::state_manager<States, Context>  manager_;
};

} // namespace fsmpp2

#endif // FSMPP2_STATE_MACHINE_HPP