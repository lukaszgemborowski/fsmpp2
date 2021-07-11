#ifndef FSMPP2_CONTEXTS_HPP
#define FSMPP2_CONTEXTS_HPP

#include <tuple>

namespace fsmpp2
{

template<class... T>
struct contexts {
    explicit contexts(T&... ctxs)
        : contexts_ {ctxs...}
    {
    }

    template<class U>
    auto& get() {
        return std::get<std::add_lvalue_reference_t<U>>(contexts_);
    }

    template<class U>
    auto const& get() const {
        return std::get<std::add_lvalue_reference_t<U>>(contexts_);
    }

private:
    std::tuple<T&...> contexts_;
};

} // namespace fsmpp2

#endif // FSMPP2_CONTEXTS_HPP