#ifndef FSMPP2_DETAIL_HPP
#define FSMPP2_DETAIL_HPP

#include "fsmpp2/meta.hpp"

namespace fsmpp2::detail
{

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

} // namespace fsmpp2::detail

#endif // FSMPP2_DETAIL_HPP