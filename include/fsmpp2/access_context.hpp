#ifndef FSMPP2_ACCESS_CONTEXT_HPP
#define FSMPP2_ACCESS_CONTEXT_HPP

#include "fsmpp2/meta.hpp"
#include <tuple>

namespace fsmpp2
{

template<class C, class... D>
struct access_context final {
    using access_context_type = meta::type_list<C, D...>;

    template<
        class T,
        class = std::enable_if_t<T::template has<C> && (T::template has<D> && ...)>
    >
    access_context(T& tuple_like) noexcept
        : ctx_ {tuple_like.template get<C>(), tuple_like.template get<D>()...}
    {
    }

    template<class T>
    T& get_context() noexcept {
        return std::get<T>(ctx_);
    }

    template<class T>
    T const& get_context() const noexcept {
        return std::get<T>(ctx_);
    }

    std::tuple<C, D...> ctx_;
};

/**
 * @brief Brings the context into the State automatically.
 */
template<class C>
struct access_context<C> final {
    using access_context_type = C;

    access_context(C& c) noexcept
        : ctx_ {&c}
    {
    }

    template<
        class U,
        class = std::enable_if_t<U::template has<C>>
    >
    access_context(U& tuple_like) noexcept
        : ctx_ {tuple_like.template get<C>()}
    {}

    /**
     * @brief Retrieve a context.
     *
     * @warning Calling this method in constructor is undefined behavior (crash in practice).
     *          If you need to access the Context in the constructor, get it via State's ctor parameter.
     */
    C& get_context() noexcept {
        return ctx_;
    }

    C const& get_context() const noexcept {
        return ctx_;
    }

    // TODO: make it inaccessible for State subclass
    C& ctx_;
};

} // namespace fsmpp2

#endif // FSMPP2_ACCESS_CONTEXT_HPP