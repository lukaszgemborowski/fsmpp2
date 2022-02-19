#ifndef FSMPP2_CONTEXTS_HPP
#define FSMPP2_CONTEXTS_HPP

#include <tuple>

namespace fsmpp2
{

/**
 * A set of references to different context types.
 *
 * If there are multiple contexts used by different states this set will be used
 * to provide a proper context to a state based on its constructor argument type.
 *
 **/
template<class... T> struct contexts {
    /**
     * Capture different Context types as references.
     **/
    explicit contexts(T&... ctxs) noexcept
        : contexts_ {ctxs...}
    {
    }

    /**
     * Get context by type.
     **/
    template<class U>
    auto& get() noexcept {
        return std::get<std::add_lvalue_reference_t<U>>(contexts_);
    }

    /**
     * Get context by type.
     **/
    template<class U>
    auto const& get() const noexcept {
        return std::get<std::add_lvalue_reference_t<U>>(contexts_);
    }

    template<class U>
    static auto constexpr has = meta::type_list_has<U>(meta::type_list<T...>{});

private:
    std::tuple<T&...> contexts_;
};

} // namespace fsmpp2

#endif // FSMPP2_CONTEXTS_HPP