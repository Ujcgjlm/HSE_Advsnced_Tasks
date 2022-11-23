#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <iostream>
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
        if constexpr (std::is_convertible_v<T*, EnableSharedFromThisBase*>) {
            InitWeakThis(ptr);
        }
    }
    template <typename T2>
    SharedPtr(T2* ptr) noexcept {
        Reset(ptr);
        if constexpr (std::is_convertible_v<T2*, EnableSharedFromThisBase*>) {
            InitWeakThis(ptr);
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other.block_);
    }
    template <typename T2>
    SharedPtr(const SharedPtr<T2>& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other.block_);
    }

    SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other.block_);
        other.Reset();
    }
    template <typename T2>
    SharedPtr(SharedPtr<T2>&& other) noexcept : ptr_(other.ptr_) {
        CopyBlock(other.block_);
        other.Reset();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) noexcept : ptr_(ptr) {
        CopyBlock(other.block_);
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        CopyBlock(other.block_);
        ptr_ = other.ptr_;
    }

    template <typename T2>
    explicit SharedPtr(const WeakPtr<T2>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        CopyBlock(other.block_);
        ptr_ = other.ptr_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (other.block_ == block_) {
            return *this;
        }
        Reset();
        ptr_ = other.ptr_;
        CopyBlock(other.block_);
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (other.block_ == block_) {
            Reset();
            ptr_ = nullptr;
            return *this;
        }
        Reset();
        ptr_ = other.ptr_;
        CopyBlock(other.block_);
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
                block_->WeakAdd();
                block_->Reset();
                block_->WeakDelete();
                if (!block_->GetWeakCount()) {
                    delete block_;
                }
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

    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        e->ptr_ = static_cast<SharedPtr<Y>>(*this);
    }

    void CopyBlock(ControlBlock* block) {
        block_ = block;
        if (block_) {
            ++*block_;
        }
    }

    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class EnableSharedFromThis;

    friend class EnableSharedFromThisBase;

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
    if constexpr (std::is_convertible_v<T*, EnableSharedFromThisBase*>) {
        shared.InitWeakThis(static_cast<T*>(shared.block_->Get()));
    }
    return shared;
}

class EnableSharedFromThisBase {};

// Look for usage examples in tests and seminar
template <typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
public:
    EnableSharedFromThis() noexcept {
    }

    EnableSharedFromThis(const EnableSharedFromThis&) {
    }

    EnableSharedFromThis& operator=(const EnableSharedFromThis&) {
        return *this;
    }

    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(this->ptr_);
    }
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(this->ptr_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(this->ptr_);
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(this->ptr_);
    }

private:
    WeakPtr<T> ptr_;

    template <typename Y>
    friend class SharedPtr;
};
