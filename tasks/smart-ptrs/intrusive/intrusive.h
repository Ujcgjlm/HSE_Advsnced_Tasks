#pragma once

#include <cstddef>  // for std::nullptr_t
#include <iostream>
#include <utility>  // for std::exchange / std::swap

class SimpleCounter {
public:
    size_t IncRef() {
        return ++count_;
    }
    size_t DecRef() {
        return --count_;
    }
    size_t RefCount() const {
        return count_;
    }

private:
    size_t count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) noexcept {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    RefCounted& operator=(RefCounted) {
        return *this;
    }
    void IncRef() {
        counter_.IncRef();
    }

    void DecRef() {
        if (!counter_.DecRef()) {
            Deleter::Destroy(static_cast<Derived*>(this));
        }
    }

    size_t RefCount() const {
        return counter_.RefCount();
    }

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
public:
    // Constructors
    IntrusivePtr() noexcept = default;
    IntrusivePtr(std::nullptr_t) noexcept : IntrusivePtr() {
    }
    IntrusivePtr(T* ptr) noexcept {
        CopyRef(ptr);
    }

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) noexcept {
        CopyRef(other.ref_);
    }

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) noexcept {
        CopyRef(other.ref_);
        other.Reset();
    }

    IntrusivePtr(const IntrusivePtr& other) noexcept {
        CopyRef(other.ref_);
    }
    IntrusivePtr(IntrusivePtr&& other) noexcept {
        CopyRef(other.ref_);
        other.Reset();
    }

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        CopyRef(other.ref_);
        return *this;
    }
    IntrusivePtr& operator=(IntrusivePtr&& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        CopyRef(other.ref_);
        other.Reset();
        return *this;
    }

    // Destructor
    ~IntrusivePtr() {
        Reset();
    }

    // Modifiers
    void Reset() {
        if (ref_) {
            ref_->DecRef();
        }
        ref_ = nullptr;
    }
    void Reset(T* ptr) {
        Reset();
        CopyRef(ptr);
    }
    void Swap(IntrusivePtr& other) {
        std::swap(ref_, other.ref_);
    }

    // Observers
    T* Get() const {
        return ref_;
    }
    T& operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }
    size_t UseCount() const {
        return ref_ ? ref_->RefCount() : 0ull;
    }
    explicit operator bool() const {
        return UseCount();
    }

private:
    T* ref_ = nullptr;

    template <typename T2>
    void CopyRef(T2* ref) {
        ref_ = ref;
        if (ref_) {
            ref_->IncRef();
        }
    }

    template <typename Y>
    friend class IntrusivePtr;
};

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr<T>(new T(args...));
}
