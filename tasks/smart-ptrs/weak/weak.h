#pragma once

#include <type_traits>
#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() = default;

    WeakPtr(const WeakPtr& other) {
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
    }
    template <typename T2>
    WeakPtr(const WeakPtr<T2>& other) {
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
    }

    WeakPtr(WeakPtr&& other) {
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
        other.Reset();
    }
    template <typename T2>
    WeakPtr(WeakPtr<T2>&& other) {
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
        other.Reset();
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (other.block_ == block_) {
            return *this;
        }
        Reset();
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (other.block_ == block_) {
            Reset();
            ptr_ = nullptr;
            return *this;
        }
        Reset();
        CopyWeakBlock(other.block_);
        ptr_ = other.ptr_;
        other.Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->WeakDelete();
            if (!UseCount() && !block_->GetWeakCount()) {
                delete block_;
            }
            block_ = nullptr;
        }
        ptr_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        return block_ ? block_->GetCount() : 0ull;
    }
    bool Expired() const {
        return !UseCount();
    }
    SharedPtr<T> Lock() const {
        SharedPtr<T> shared;
        shared.CopyBlock(block_ ? block_ : nullptr);
        shared.ptr_ = ptr_;
        return shared;
    }

private:
    T* ptr_ = nullptr;
    ControlBlock* block_ = nullptr;

    void CopyWeakBlock(ControlBlock* block) {
        block_ = block;
        if (block_) {
            block_->WeakAdd();
        }
    }

    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class SharedPtr;
};
