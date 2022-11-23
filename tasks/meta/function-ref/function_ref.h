#pragma once

#include <iostream>
#include <type_traits>
#include <utility>
#include <functional>

template <typename Signature>
class FunctionRef;

template <typename R, typename... Args>
class FunctionRef<R(Args...)> {
public:
    FunctionRef() = delete;

    FunctionRef(const FunctionRef<R(Args...)>&) = default;
    FunctionRef(FunctionRef<R(Args...)>&&) = default;

    auto& operator=(FunctionRef<R(Args...)> func_ref) {
        Swap(*this, func_ref);
        return *this;
    }
    auto& operator=(FunctionRef<R(Args...)>&& func_ref) {
        Swap(*this, func_ref);
        return *this;
    }

    template <typename T>
    FunctionRef(T&& obj) : obj_(reinterpret_cast<void*>(&obj)) {
        func_ = [](void* obj, Args&&... args) -> R {
            return static_cast<R>(std::invoke(*reinterpret_cast<std::remove_reference_t<T>*>(obj),
                                              std::forward<Args>(args)...));
        };
    }

    template <typename T>
    auto& operator=(T&& obj) {
        obj_ = reinterpret_cast<void*>(&obj);
        func_ = [](void* obj, Args&&... args) -> R {
            return static_cast<R>(std::invoke(*reinterpret_cast<std::remove_reference_t<T>*>(obj),
                                              std::forward<Args>(args)...));
        };
        return *this;
    }

    R operator()(Args&&... args) const {
        return func_(obj_, std::forward<Args>(args)...);
    }

    void Swap(FunctionRef<R(Args...)>& a, FunctionRef<R(Args...)>& b) {
        std::swap(a, b);
    }

private:
    void* obj_ = nullptr;
    R (*func_)(void*, Args&&...) = nullptr;

    template <typename Y>
    friend class FunctionRef;
};
