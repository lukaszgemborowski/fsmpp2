#ifndef FSMPP2_TRANSITIONS_HPP
#define FSMPP2_TRANSITIONS_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/detail.hpp"

namespace fsmpp2
{

/**
 * Event handler return type.
 *
 * It indicates possible outcomes from an event handler.
 * API of this class used internally in fsmpp2 framework.
 **/
template<class... S>
class transitions {
private:
    enum class result {
        not_handled,
        handled,
        transition
    };

public:
    explicit transitions(std::size_t i)
        : idx {i}
        , outcome {result::transition}
    {}

    template<class U>
    explicit transitions(transitions<U>)
        : idx {meta::type_list_index<U>(list{})}
        , outcome {result::transition}
    {
    }

    transitions(transitions<> const& t)
        : idx {sizeof...(S)}
        , outcome {t.outcome}
    {
    }

    explicit  transitions(detail::handled)
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    explicit transitions(detail::not_handled)
        : idx {sizeof...(S)}
        , outcome {result::not_handled}
    {}

    bool is_transition() const {
        return outcome == result::transition;
    }

    bool is_handled() const {
        return outcome == result::handled;
    }

    using list = meta::type_list<S...>;
    std::size_t idx;
    result outcome;
};

} // namespace fsmpp2

#endif // FSMPP2_TRANSITIONS_HPP