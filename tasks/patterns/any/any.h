#pragma once

#include <functional>
#include <memory>

class Any {
public:
    Any() {
    }

    template <class T>
    Any(const T& value) : holder_(std::make_shared<AnyHolder<T>>(value)) {
    }

    template <class T>
    Any& operator=(const T& value) {
        holder_ = std::make_shared<AnyHolder<T>>(value);
        return *this;
    }

    Any(const Any& rhs) : holder_(rhs.holder_) {
    }
    Any& operator=(Any rhs) {
        Swap(rhs);
        return *this;
    }
    ~Any() {
    }

    bool Empty() const {
        return !holder_.use_count();
    }

    void Clear() {
        holder_ = nullptr;
    }
    void Swap(Any& rhs) {
        std::swap(holder_, rhs.holder_);
    }

    template <class T>
    const T& GetValue() const {
        if (holder_->GetType() != typeid(T)) {
            throw std::bad_cast();
        }

        return static_cast<AnyHolder<T>*>(holder_.get())->value_;
    }

private:
    struct AnyBaseHolder {
        virtual ~AnyBaseHolder() = default;
        virtual const std::type_info& GetType() const = 0;
    };

    template <typename T>
    struct AnyHolder : AnyBaseHolder {
        AnyHolder(const T& value) : value_(value) {
        }

        const std::type_info& GetType() const override {
            return typeid(value_);
        }

        T value_;
    };

    std::shared_ptr<AnyBaseHolder> holder_;
};
