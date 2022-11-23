#pragma once

#include <pthread.h>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <type_traits>

class TryImpl {
public:
    TryImpl() noexcept {
    }

    void Throw() const {
        if (except_) {
            std::rethrow_exception(except_);
        }
        throw std::logic_error("No exception");
    }

    bool IsFailed() const {
        return except_ ? true : false;
    }

private:
    std::exception_ptr except_ = nullptr;

    template <typename Y>
    friend class Try;
};

template <class T>
class Try : public TryImpl {
public:
    Try() noexcept {
        is_empty_ = true;
    }

    template <typename Y>
    Try(const Y& data) noexcept {
        if constexpr (std::is_base_of_v<std::exception, Y>) {
            except_ = std::make_exception_ptr(data);
        } else {
            if constexpr (std::is_same_v<Y, std::exception_ptr>) {
                except_ = data;
            } else {
                data_ = static_cast<T>(data);
            }
        }
    }

    const T& Value() const {
        if (IsFailed()) {
            Throw();
        }
        if (is_empty_) {
            throw std::logic_error("Object is empty");
        }
        return data_;
    }

private:
    T data_;
    bool is_empty_ = false;
};

template <>
class Try<void> : public TryImpl {
public:
    Try() noexcept {
    }

    template <typename Y>
    Try(const Y& e) noexcept {
        if constexpr (std::is_base_of_v<std::exception, Y>) {
            except_ = std::make_exception_ptr(e);
        } else {
            except_ = e;
        }
    }
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));

    std::exception except;

    try {
        if constexpr (std::is_void_v<ReturnType>) {
            func(args...);
            return Try<void>();
        } else {
            return Try<ReturnType>(func(args...));
        }
    } catch (const std::exception& e) {
        return Try<ReturnType>(std::current_exception());
    } catch (const char* e) {
        return Try<ReturnType>(std::make_exception_ptr(std::logic_error(e)));
    } catch (int e) {
        return Try<ReturnType>(std::make_exception_ptr(std::logic_error(std::strerror(e))));
    } catch (...) {
        return Try<ReturnType>(std::make_exception_ptr(std::logic_error("Unknown exception")));
    }
}
