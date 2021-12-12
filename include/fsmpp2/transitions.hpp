#ifndef FSMPP2_TRANSITIONS_HPP
#define FSMPP2_TRANSITIONS_HPP

#include "fsmpp2/meta.hpp"

namespace fsmpp2
{
namespace detail
{

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

} // namespace fsmpp2::detail

/**
 * @brief Event handler return type.
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
    /**
     * @brief Construct a new transitions object
     *
     * This constructor is used only when returning from state::transition() method
     * to indicate the destination state. It is then converted to corresponding
     * transitions<S1, S2...> type at return.
     */
    explicit transitions()
        : idx {0}
        , outcome {result::transition}
    {}

    /**
     * @brief Construct a new transitions object
     *
     * This constructor is used when converting transitions<S> object returned
     * from state::transition() call. Idx is calculated to indicate index of the
     * target state.
     *
     * @tparam U destination state
     */
    template<class U>
    explicit transitions(transitions<U>)
        : idx {meta::type_list_index<U>(list{})}
        , outcome {result::transition}
    {
    }

    /**
     * @brief Construct a new transitions object
     *
     * This constructor is used when converting return value from state::handled/not_handled()
     *
     * @param t transitions object returned from state::handled/not_handled()
     */
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

    /**
     * @brief Check if this object is a transition type.
     *
     * @return true if the object should result in transition
     * @return false if the object is not resulting in transition
     */
    bool is_transition() const {
        return outcome == result::transition;
    }

    /**
     * @brief Check if the event was handled.
     *
     * @return true event was handled by the state.
     * @return false event was not handled. Possibly propagate event up in the SM.
     */
    bool is_handled() const {
        return outcome == result::handled;
    }

    using list = meta::type_list<S...>;

    std::size_t idx;
    result outcome;
};

} // namespace fsmpp2

#endif // FSMPP2_TRANSITIONS_HPP