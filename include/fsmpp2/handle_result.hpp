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

} // namespace fsmpp2

#endif // FSMPP2_HANDLE_RESULT_HPP