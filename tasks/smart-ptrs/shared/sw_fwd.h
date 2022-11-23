#pragma once

#include <exception>

namespace {

class ControlBlock {
public:
    ControlBlock(std::nullptr_t) = delete;
    ControlBlock& operator=(const ControlBlock& other) = delete;
    ControlBlock& operator=(const ControlBlock&& other) = delete;

    ControlBlock() noexcept : counter_(1) {
    }

    virtual ~ControlBlock() {
    }

    void operator++() {
        ++counter_;
    }

    void operator--() {
        --counter_;
    }

    virtual void* Get() {
        return nullptr;
    }

    int GetCount() const {
        return counter_;
    }

protected:
    int counter_;
};

template <typename T2>
class ControlBlockWithObject : public ControlBlock {
public:
    template <typename... Args>
    ControlBlockWithObject(Args&&... args) {
        new (storage_) T2(std::forward<Args>(args)...);
    }

    virtual ~ControlBlockWithObject() {
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
        delete ptr_;
    }

    void* Get() override {
        return ptr_;
    }

private:
    T2* ptr_;
};

}  // unnamed namespace


class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
