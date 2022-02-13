#ifndef FSMPP2_TRANSITIONS_HPP
#define FSMPP2_TRANSITIONS_HPP

#include "fsmpp2/meta.hpp"
#include "fsmpp2/handle_result.hpp"

namespace fsmpp2
{

/**
 * @brief Event handler return type.
 *
 * It indicates possible outcomes from an event handler. It is also
 * used to carry on information about event handler outcome. As its
 * created from state->handle(ev) result.
 */
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
     * @brief Construct new object from empty-listed transactions object.
     *
     * This is a case when handler declares a return type:
     *      transitions<A, B, C> handle(Foo);
     * but calls handled() or not_handled() which returns exactly transitions<> type
     * which needs to be converted here to transitions<A, B, C>.
     *
     */
    constexpr transitions(transitions<> const& rhs) noexcept
        : idx {0}
        , outcome {rhs.outcome}
    {
    }

    /**
     * @brief Construct a new transitions object directly from detail::transition<>
     *
     * @tparam T state
     */
    template<class T>
    transitions(detail::transition<T>) noexcept
        : idx {meta::type_list_index<T>(list{})}
        , outcome {result::transition}
    {
    }

    /**
     * @brief Construct a new transitions object indicating handled event
     */
    transitions(detail::handled) noexcept
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    /**
     * @brief Construct a new transitions object indicating not handled event
     */
    transitions(detail::not_handled) noexcept
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

    std::size_t const idx;
    result      const outcome;
};

} // namespace fsmpp2

#endif // FSMPP2_TRANSITIONS_HPP