#pragma once
#include <cstddef>
#include <string>
#include <vector>

class COWVector {
public:
    COWVector() {
        state_ = new State();
    }

    ~COWVector() {
        if (!state_->ref_count) {
            delete state_;
        } else {
            --state_->ref_count;
        }
    }

    COWVector(const COWVector& other) {
        state_ = other.state_;
        ++state_->ref_count;
    }

    COWVector& operator=(const COWVector& other) {
        if (&other == this) {
            return *this;
        }

        if (state_) {
            if (!state_->ref_count) {
                delete state_;
            } else {
                --state_->ref_count;
            }
        }
        state_ = other.state_;
        ++state_->ref_count;
        return *this;
    }

    void MaybeDeepCopy() {
        if (state_->ref_count) {
            --state_->ref_count;
            state_ = new State(0, state_->vector);
        }
    }

    size_t Size() const {
        return state_->vector.size();
    };

    void Resize(size_t size) {
        MaybeDeepCopy();
        state_->vector.resize(size);
    }

    const std::string& Get(size_t at) const {
        return state_->vector[at];
    }
    const std::string& Back() const {
        return state_->vector.back();
    }

    void PushBack(const std::string& value) {
        MaybeDeepCopy();
        state_->vector.push_back(value);
    }

    void Set(size_t at, const std::string& value) {
        MaybeDeepCopy();
        state_->vector[at] = value;
    }

private:
    struct State {
        State(size_t ref_count = 0, std::vector<std::string> vector = {})
            : ref_count(ref_count), vector(vector) {
        }
        size_t ref_count;
        std::vector<std::string> vector;
    };

    State* state_ = nullptr;
};
