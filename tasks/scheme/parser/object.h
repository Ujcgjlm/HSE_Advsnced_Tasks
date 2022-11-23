#pragma once

#include <cstddef>
#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) : value_(value) {
    }

    int GetValue() const {
        return value_;
    }

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol(std::string name) : name_(std::move(name)) {
    }

    const std::string& GetName() const {
        return name_;
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> left, std::shared_ptr<Object> right) : left_(left), right_(right) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return left_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return right_;
    }

private:
    std::shared_ptr<Object> left_;
    std::shared_ptr<Object> right_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj) != nullptr;
}
