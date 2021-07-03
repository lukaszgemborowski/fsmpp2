#include <tuple>
#include <functional>
#include <variant>
#include <array>
#include <iostream>

// framework
namespace sm
{
namespace detail
{

template<class... T> struct type_list {};

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

template<std::size_t I, class F, class... T>
struct type_list_type<I, type_list<F, T...>> {
    using type = typename type_list_type_impl<0, I, F, T...>::type;
};


template<class... F>
constexpr auto type_list_size(type_list<F...>)
{
    return sizeof...(F);
}

template<class X, class F, class ... T>
constexpr bool type_list_has(type_list<F, T...> l)
{
    auto idx = type_list_index<X>(l);
    return idx < type_list_size(l);
}

template<class... T>
constexpr auto storage_for(type_list<T...>)
{
    static_assert(sizeof...(T) > 0);

    std::array<std::size_t, sizeof...(T)> arr{sizeof(T)...};
    return *std::max_element(arr.begin(), arr.end());
}

// sm specific
template<class S, class E>
concept EventHandler = requires(S s, E e) {
    s.handle(e);
};

struct handled {};
struct not_handled {};
template<class T> struct transition { using type = T; };

struct NullContext {};
struct NoSubStates {
    template<class E> auto handle(E const&) { return false; }
};

} // detail

struct event {};

template<class... S>
struct transitions {
    using list = detail::type_list<S...>;
    std::size_t idx;

    enum class result {
        not_handled,
        handled,
        transition
    } outcome;

    transitions(std::size_t i)
        : idx {i}
        , outcome {result::transition}
    {}

    template<class U>
    transitions(transitions<U>)
        : idx {detail::type_list_index<U>(list{})}
        , outcome {result::transition}
    {
    }

    transitions(detail::handled)
        : idx {sizeof...(S)}
        , outcome {result::handled}
    {}

    transitions(detail::not_handled)
        : idx {sizeof...(S)}
        , outcome {result::not_handled}
    {}
};

template<class Context = detail::NullContext, class SubStates = detail::NoSubStates>
struct state {
    using context_type = Context;

    template<class S>
    auto transition() const {
        return transitions<S>{0};
    }

    auto not_handled() const {
        return detail::not_handled{};
    }

    auto handled() const {
        return detail::handled{};
    }

    SubStates substates_;
};

template<class... States>
struct state_instance
{
    template<class F, class ...>
    struct state_context_type {
        using type = typename F::context_type;
    };

public:
    using context_type = typename state_context_type<States...>::type;
    using type_list = detail::type_list<States...>;

    template<class State>
    void create() {
        static_assert(detail::type_list_has<State>(type_list{}), "state is not in set");

        new (storage_) State ();
        index_ = detail::type_list_index<State>(type_list{});
    }

    void destroy() {
        apply(
            [this]<typename T>(T *ptr) {
                ptr->~T();
                index_ = sizeof...(States);
            }
        );
    }

    template<class F>
    void apply(F func) {
        apply(func, std::make_index_sequence<sizeof...(States)>{});
    }

private:
    template<class F, std::size_t... Idx>
    void apply(F func, std::index_sequence<Idx...>) {
        bool executed = false;
        (apply_one<Idx>(executed, func), ...);
    }

    template<std::size_t I, class F>
    void apply_one(bool& executed, F func) {
        if (!executed && I == index_) {
            using state_type = typename detail::type_list_type<I, type_list>::type;
            func(reinterpret_cast<state_type *>(storage_));
            executed = true;
        }
    }

private:
    // TODO: extract storage type to separate class
    unsigned char storage_[detail::storage_for(detail::type_list<States...>{})];
    std::size_t index_ = sizeof...(States);
    context_type context_;
};

template<class First, class... States>
struct states
{
public:
    using type_list = detail::type_list<First, States...>;

    // TODO: verify that all states use the same context_type, otherwise static_assert
    using context_type = typename First::context_type;

    states()
    {
        states_.template create<First>();
    }

    ~states()
    {
        states_.destroy();
    }

    template<class E>
    auto handle(E const& e) {
        auto result = false;
        states_.apply(
            [this, &e, &result](auto *ptr) { result = handle(*ptr, e); }
        );

        return result;
    }

private:
    template<class S, class E>
    bool handle(S &state, E const& e) requires detail::EventHandler<S, E> {
        if (!state.substates_.handle(e)) {
            return handle_result(state.handle(e));
        } else {
            return true;
        }
    }

    template<class S, class E>
    bool handle(S& state, E const& e) {
        if (state.substates_.handle(e)) {
            return true;
        } else {
            return false;
        }
    }

    bool handle_result(detail::not_handled) {
        return false;
    }

    bool handle_result(detail::handled) {
        return true;
    }

    template<class... T>
    bool handle_result(transitions<T...> t) {
        // TODO: verify T... are in First,States...
        handle_transition(t, std::make_index_sequence<sizeof...(T)>{});
        return true; // assume true
    }

    template<class Transition, std::size_t... I>
    void handle_transition(Transition trans, std::index_sequence<I...>) {
        (handle_transition_impl<I>(trans), ...);
    }

    template<std::size_t I, class Transition>
    void handle_transition_impl(Transition trans) {
        if (trans.idx == I) {
            states_.destroy();

            using transition_type_list = typename Transition::list;
            using type_at_index = typename detail::type_list_type<I, transition_type_list>::type;

            if constexpr (detail::type_list_has<type_at_index>(type_list{})) {
                states_.template create<type_at_index>();
            }
        }
    }

private:
    state_instance<First, States...> states_;
};

} // sm


struct Ev1 : sm::event {
    int value = 0;
};
struct Ev2 : sm::event {
    int value = 0;
};

class A;
class B;
class C;

struct A : sm::state<> {
    A() {
        std::cout << "A enter" << std::endl;
    }

    ~A() {
        std::cout << "A exit" << std::endl;
    }

    auto handle(Ev1 const&) -> sm::transitions<B, C>
    {
        std::cout << "A::Ev1" << std::endl;
        return transition<B>();
    }
};

struct B1 : sm::state<>
{
    B1() {
        std::cout << "B1" << std::endl;
    }

    auto handle(Ev1 const&)
    {
        std::cout << "B1::Ev1" << std::endl;
        return handled();
    }
};

struct B2 : sm::state<>
{
    B2() {
        std::cout << "B2" << std::endl;
    }
};

struct B : sm::state<sm::detail::NullContext, sm::states<B1, B2>> {
    B() {
        std::cout << "B enter" << std::endl;
    }

    ~B() {
        std::cout << "B exit" << std::endl;
    }

    auto handle(Ev2 const& e) -> sm::transitions<A, C>
    {
        std::cout << "B::Ev2" << std::endl;

        if (e.value == 1) {
            return transition<A>();
        } else if (e.value == 2) {
            return transition<C>();
        }

        return not_handled();
    }
};

struct C : sm::state<> {
    C() {
        std::cout << "C enter" << std::endl;
    }

    ~C() {
        std::cout << "C exit" << std::endl;
    }
};


int main()
{
    sm::states<B> states;
    //states.handle(Ev1{}); // A->B(B1)
    states.handle(Ev1{}); // B(B1 -> B2)
}