#pragma once

#include <utility>

template <typename Callback>
class CallbackStorage {
public:
    explicit CallbackStorage(Callback callback) {
        ::new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    ~CallbackStorage() {
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }

    void Destroy() {
        GetCallback().~Callback();
        is_alive_ = false;
    }

    bool IsAlive() {
        return is_alive_;
    }

private:
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
    bool is_alive_ = true;
};

template <typename Callback>
class Defer final {
public:
    Defer() = delete;

    Defer(Callback callback) : callback_(std::move(callback)) {
    }

    ~Defer() {
        if (callback_.IsAlive()) {
            std::move(*this).Invoke();
        }
    }

    void Cancel() {
        callback_.Destroy();
    }

    void Invoke() && {
        std::move(callback_.GetCallback())();
        callback_.Destroy();
    }

private:
    CallbackStorage<Callback> callback_;
};

template <class T>
Defer(T t) -> Defer<T>;
