#ifndef FSMPP2_TRANSITIONS_HPP
#define FSMPP2_TRANSITIONS_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail.hpp"

namespace fsmpp2
{

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

} // namespace fsmpp2

#endif // FSMPP2_TRANSITIONS_HPP