#pragma once

#include "compressed_pair.h"
#include "unique/deleters.h"

#include <cstddef>  // std::nullptr_t
#include <type_traits>
#include <utility>

template <typename T>
struct Slug {
    Slug() noexcept = default;

    template <typename T2>
    Slug(const Slug<T2>&) noexcept {
    }

    inline void operator()(T* ptr) {
        delete ptr;
    }
};

template <typename T>
struct Slug<T[]> {
    Slug() noexcept = default;

    template <typename T2>
    Slug(const Slug<T2>&) noexcept {
    }

    inline void operator()(T* ptr) {
        delete[] ptr;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept : data_(ptr, Deleter()) {
    }
    template <typename T2, typename Deleter2>
    UniquePtr(T2* ptr, Deleter2&& deleter) noexcept : data_(ptr, std::forward<Deleter2>(deleter)) {
    }

    template <typename T2, typename Deleter2>
    UniquePtr(UniquePtr<T2, Deleter2>&& other) noexcept
        : data_(std::move(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::move(other.GetDeleter());
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto buf = Get();
        data_.GetFirst() = nullptr;
        return buf;
    }
    void Reset(T* ptr = nullptr) {
        auto buf = Release();
        data_.GetFirst() = ptr;
        if (buf) {
            GetDeleter()(buf);
        }
    }
    void Swap(UniquePtr& other) noexcept {
        using std::swap;
        swap(data_, other.data_);я
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return Get();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

template <typename Deleter>
class UniquePtr<void, Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(void* ptr = nullptr) noexcept : data_(ptr, Deleter()) {
    }
    template <typename T2, typename Deleter2>
    UniquePtr(T2* ptr, Deleter2&& deleter) noexcept : data_(ptr, std::forward<Deleter2>(deleter)) {
    }

    template <typename T2, typename Deleter2>
    UniquePtr(UniquePtr<T2, Deleter2>&& other) noexcept
        : data_(std::move(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::move(other.GetDeleter());
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void* Release() {
        auto buf = Get();
        data_.GetFirst() = nullptr;
        return buf;
    }
    void Reset(void* ptr = nullptr) {
        auto buf = Release();
        if (buf) {
            GetDeleter()(buf);
        }
        data_.GetFirst() = ptr;
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    void* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return Get();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    void* operator->() const {
        return Get();
    }

private:
    CompressedPair<void*, Deleter> data_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept : data_(ptr, Deleter()) {
    }
    template <typename T2, typename Deleter2>
    UniquePtr(T2* ptr, Deleter2&& deleter) noexcept : data_(ptr, std::forward<Deleter2>(deleter)) {
    }

    template <typename T2, typename Deleter2>
    UniquePtr(UniquePtr<T2, Deleter2>&& other) noexcept
        : data_(std::move(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::move(other.GetDeleter());
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        auto buf = Get();
        data_.GetFirst() = nullptr;
        return buf;
    }
    void Reset(T* ptr = nullptr) {
        auto buf = Release();
        if (buf) {
            GetDeleter()(buf);
        }
        data_.GetFirst() = ptr;
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return Get();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator[](size_t i) const {
        return Get()[i];
    }
    T& operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }

private:
    CompressedPair<T*, Deleter> data_;
};
