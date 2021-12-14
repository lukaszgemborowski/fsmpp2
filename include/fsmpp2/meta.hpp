#ifndef FSMPP2_META_HPP
#define FSMPP2_META_HPP

// std::size_t
#include <cstddef>

// std::is_same_v
#include <type_traits>

namespace fsmpp2::meta
{

/**
 * @brief List of types.
 *
 * This is basic entity for any other meta operations.
 */
template<class... T> struct type_list {};

/**
 * @brief Find index of X type in provided type_list<>
 */
template<class X, class F, class ... T>
constexpr auto type_list_index(type_list<F, T...>, std::size_t idx = 0)
{
    if (std::is_same_v<F, X>) {
        return idx;
    } else {
        if constexpr (sizeof...(T) > 0) {
            return type_list_index<X>(type_list<T...>{}, idx + 1);
        } else {
            return idx + 1;
        }
    }
}

template<class X>
constexpr auto type_list_index(type_list<>, std::size_t idx = 0)
{
    return 0;
}

template<std::size_t I, std::size_t Target, class F, class... T>
struct type_list_type_impl {
    using type = typename type_list_type_impl<I + 1, Target, T...>::type;
};

template<std::size_t I, class F, class... T>
struct type_list_type_impl<I, I, F, T...> {
    using type = F;
};

template<std::size_t I, class F, class... T>
struct type_list_type {};

/**
 * @brief Finds a type at index I in provided type_list<>
 */
template<std::size_t I, class F, class... T>
struct type_list_type<I, type_list<F, T...>> {
    using type = typename type_list_type_impl<0, I, F, T...>::type;
};

/**
 * @brief Caluclate size of type_list.
 */
template<class... F>
constexpr auto type_list_size(type_list<F...>)
{
    return sizeof...(F);
}

/**
 * @brief Check wheter type X is in the provided type_list<>
 */
template<class X, class F, class ... T>
constexpr bool type_list_has(type_list<F, T...> l)
{
    auto idx = type_list_index<X>(l);
    return idx < type_list_size(l);
}

/**
 * @brief Get first type in provided type_list<>
 */
template<class List> struct type_list_first {};
template<class First, class... Tail>
struct type_list_first<type_list<First, Tail...>> {
    using type = First;
};

template<class, template<typename> typename>
struct type_list_transform;

/**
 * @brief Transforms provided type_list by applying meta-function F
 *
 * Every element of type_list will be modified by "calling" F<T>::type.
 */
template<class... T, template<typename> typename F>
struct type_list_transform<type_list<T...>, F> {
    using result = type_list< typename F<T>::type ... >;
};

template<class, template<typename...> typename> struct type_list_rename;
template<class... T, template<typename...> typename F>
struct type_list_rename<type_list<T...>, F> {
    using result = F<T...>;
};

/**
 * @brief Append a type at the begining of the type_list.
 */
template<class, class> struct type_list_push_front;
template<class... E, class T> struct type_list_push_front<type_list<E...>, T> {
    using result = type_list<T, E...>;
};

} // namespace fsmpp2::meta

#endif // FSMPP2_META_HPP