#pragma once

#include <cassert>

template <class T>
class Deleter {
public:
    Deleter() = default;

    Deleter(int tag) : tag_(tag) {
    }

    Deleter(const Deleter&) = delete;

    Deleter(Deleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    Deleter& operator=(const Deleter&) = delete;

    Deleter& operator=(Deleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    ~Deleter() = default;

    int GetTag() const {
        return tag_;
    }

    void operator()(T* p) const {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void<T>::value);
        delete p;
    }

    bool IsConst() const {
        return true;
    }

    bool IsConst() {
        return false;
    }

private:
    int tag_ = 0;
};

template <class T>
class Deleter<T[]> {
public:
    Deleter() = default;

    Deleter(int tag) : tag_(tag) {
    }

    Deleter(const Deleter&) = delete;

    Deleter(Deleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    Deleter& operator=(const Deleter&) = delete;

    Deleter& operator=(Deleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    ~Deleter() = default;

    int GetTag() const {
        return tag_;
    }

    void operator()(T* p) const {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void<T>::value);
        delete[] p;
    }

    bool IsConst() const {
        return true;
    }

    bool IsConst() {
        return false;
    }

private:
    int tag_ = 0;
};

template <typename T>
class CopyableDeleter {
public:
    CopyableDeleter() = default;

    CopyableDeleter(int tag) : tag_(tag) {
    }

    CopyableDeleter(const CopyableDeleter&) = default;

    CopyableDeleter(CopyableDeleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    CopyableDeleter& operator=(const CopyableDeleter&) = default;

    CopyableDeleter& operator=(CopyableDeleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    ~CopyableDeleter() = default;

    int GetTag() const {
        return tag_;
    }

    void operator()(T* p) const {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void<T>::value);
        delete p;
    }

    bool IsConst() const {
        return true;
    }

    bool IsConst() {
        return false;
    }

private:
    int tag_ = 0;
};
