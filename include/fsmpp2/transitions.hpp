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
     * @brief Construct a new transitions object from handle_result<State>
     *
     * @tparam T state
     */
    template<class T>
    transitions(handle_result<T>) noexcept
        : idx {meta::type_list_index<typename T::type>(list{})}
        , outcome {result::transition}
    {
    }

    /**
     * @brief Construct a new transitions object indicating handled event
     */
    transitions(handle_result<detail::handled>) noexcept
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    /**
     * @brief Construct a new transitions object indicating not handled event
     */
    transitions(handle_result<detail::not_handled>) noexcept
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