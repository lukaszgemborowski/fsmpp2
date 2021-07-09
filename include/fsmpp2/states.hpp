#ifndef FSMPP2_STATES_HPP
#define FSMPP2_STATES_HPP

#include "fsmpp2/meta.hpp"

namespace fsmpp2
{

template<class... S>
struct states {
    using type_list = meta::type_list<S...>;
    static constexpr auto count = meta::type_list_size(type_list{});
};

} // namespace fsmpp2

#endif // FSMPP2_STATES_HPP