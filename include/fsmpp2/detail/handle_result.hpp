#ifndef FSMPP2_HANDLE_RESULT_HPP
#define FSMPP2_HANDLE_RESULT_HPP

namespace fsmpp2::detail
{

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

} // namespace fsmpp2::detail

#endif // FSMPP2_HANDLE_RESULT_HPP