#ifndef FSMPP2_HANDLE_RESULT_HPP
#define FSMPP2_HANDLE_RESULT_HPP

namespace fsmpp2
{
namespace detail
{

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

} // namespace fsmpp2::detail

/**
 * @brief Result of state event handler.
 *
 * @tparam T type of the result
 */
template<class T>
class handle_result{
    // TODO: static_assert for handled/not_handled/transition
};

} // namespace fsmpp2

#endif // FSMPP2_HANDLE_RESULT_HPP