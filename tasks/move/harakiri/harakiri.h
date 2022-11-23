#pragma once

#include <string>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() {
    }
    virtual const std::string operator()() const&& = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback(const std::string& str) : str_(str) {
    }

    virtual ~AwesomeCallback() {
    }

    virtual const std::string operator()() const&& override {
        const auto str = str_ + "awesomeness";
        delete this;
        return str;
    }

private:
    const std::string str_;
};
