#ifndef FSMPP2_CONTEXT_HPP
#define FMSPP2_CONTEXT_HPP

#include <optional>

namespace fsmpp2::detail
{

template<class T>
class context {
public:
    context()
        : instance_ {std::in_place_t{}}
        , instance_ptr_ {&instance_.value()}
    {
    }

    template<class... Args>
    context(Args&& ...args)
        : instance_ {std::in_place_t{}, args...}
        , instance_ptr_ {&instance_.value()}
    {
    }

    context(T& ctx)
        : instance_ptr_ {&ctx}
    {
    }

    // no copy or move
    context(context<T> const&) = delete;
    context(context<T> &&) = delete;
    context<T>& operator=(context<T> const&) = delete;
    context<T>& operator=(context<T> &&) = delete;

    T& value() {
        return *instance_ptr_;
    }

    T const& value() const {
        return *instance_ptr_;
    }

private:
    std::optional<T> instance_;
    T* instance_ptr_ = nullptr;
};

} // namespace fsmpp2::detail

#endif // FMSPP2_CONTEXT_HPP