#pragma once

#include <cstddef>
#include <exception>

<<<<<<< Updated upstream
// Instead of std::bad_weak_ptr
=======
namespace {

class ControlBlock {
public:
    ControlBlock(std::nullptr_t) = delete;
    ControlBlock& operator=(const ControlBlock& other) = delete;
    ControlBlock& operator=(const ControlBlock&& other) = delete;

    ControlBlock() noexcept : counter_(1), weak_counter_(0) {
    }

    virtual ~ControlBlock() {
    }

    virtual void Reset() {
    }

    void operator++() {
        ++counter_;
    }

    void operator--() {
        --counter_;
    }

    void WeakDelete() {
        --weak_counter_;
    }

    void WeakAdd() {
        ++weak_counter_;
    }

    virtual void* Get() {
        return nullptr;
    }

    int GetCount() const {
        return counter_;
    }

    int GetWeakCount() {
        return weak_counter_;
    }

protected:
    int counter_ = 1;
    int weak_counter_ = 0;
};

template <typename T2>
class ControlBlockWithObject : public ControlBlock {
public:
    template <typename... Args>
    ControlBlockWithObject(Args&&... args) {
        new (storage_) T2(std::forward<Args>(args)...);
    }

    virtual ~ControlBlockWithObject() {
    }

    void Reset() override {
        reinterpret_cast<T2*>(Get())->~T2();
    }

    void* Get() override {
        return &storage_;
    }

private:
    alignas(T2) char storage_[sizeof(T2)];
};

template <typename T2>
class ControlBlockWithPtr : public ControlBlock {
public:
    template <typename T3>
    ControlBlockWithPtr(T3* ptr) : ptr_(ptr) {
    }

    virtual ~ControlBlockWithPtr() {
        if (ptr_) {
            Reset();
        }
    }

    void Reset() override {
        delete ptr_;
        ptr_ = nullptr;
    }

    void* Get() override {
        return ptr_;
    }

private:
    T2* ptr_;
};

}  // unnamed namespace

>>>>>>> Stashed changes
class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

class EnableSharedFromThisBase;