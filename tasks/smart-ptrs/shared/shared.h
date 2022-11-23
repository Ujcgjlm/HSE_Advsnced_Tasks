#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <type_traits>
#include <utility>

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() noexcept = default;
    SharedPtr(std::nullptr_t) noexcept : SharedPtr() {
    }
    explicit SharedPtr(T* ptr) noexcept {
        Reset(ptr);
    }
    template <typename T2>
    SharedPtr(T2* ptr) noexcept {
        Reset(ptr);
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other);
    }
    template <typename T2>
    SharedPtr(const SharedPtr<T2>& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other);
    }

    SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other);
        other.Reset();
    }
    template <typename T2>
    SharedPtr(SharedPtr<T2>&& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other);
        other.Reset();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) noexcept : ptr_(ptr) {
        CopyBlock(other);
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) noexcept;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        Reset();
        ptr_ = other.ptr_;
        CopyBlock(other);
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        Reset();
        ptr_ = other.ptr_;
        CopyBlock(other);
        other.Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() noexcept {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            --*block_;
            if (!block_->GetCount()) {
                delete block_;
            }
            block_ = nullptr;
        }
        ptr_ = nullptr;
    }
    template <typename T2>
    void Reset(T2* ptr) {
        Reset();
        block_ = new ControlBlockWithPtr<T2>(ptr);
    }
    void Swap(SharedPtr& other) noexcept {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return (ptr_) ? ptr_ : ((block_) ? reinterpret_cast<T*>(block_->Get()) : nullptr);
    }
    T& operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }
    size_t UseCount() const {
        return block_ ? block_->GetCount() : 0ull;
    }
    explicit operator bool() const {
        return Get();
    }

private:
    T* ptr_ = nullptr;
    ControlBlock* block_ = nullptr;

    template <typename T2>
    void CopyBlock(const SharedPtr<T2>& other) {
        block_ = other.block_;
        if (block_) {
            ++*block_;
        }
    }

    template <typename Y>
    friend class SharedPtr;

    template <typename T2, typename... Args>
    friend SharedPtr<T2> MakeShared(Args&&... args);
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    SharedPtr<T> shared;
    shared.block_ = new ControlBlockWithObject<T>(std::forward<Args>(args)...);
    return shared;
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
