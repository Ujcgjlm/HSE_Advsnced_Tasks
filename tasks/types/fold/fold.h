#pragma once

#include <type_traits>
#include <vector>
struct Sum {
    int operator()(int a, int b) {
        return a + b;
    }
};

struct Prod {
    int operator()(int a, int b) {
        return a * b;
    }
};

struct Concat {
    std::vector<int> operator()(std::vector<int> a, const std::vector<int>& b) {
        a.insert(a.end(), b.begin(), b.end());
        return a;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    for (auto it = first; it != last; ++it) {
        init = func(init, *it);
    }
    return init;
}

class Length {
public:
    Length() = delete;
    Length(int* ptr) : len_(ptr) {
    }

    template <typename T>
    T operator()(T&& a, T&& b) {
        *len_ += (a != std::decay_t<T>()) + (b != std::decay_t<T>());
        return a;
    }

private:
    int* len_;
};
