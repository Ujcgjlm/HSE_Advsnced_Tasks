#pragma once

#include <functional>
#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
public:
    ReverseBinaryFunctor() = delete;
    ReverseBinaryFunctor(Functor f) : f_(f) {
    }

    template <typename T>
    inline bool operator()(const T& a, const T& b) const {
        return f_(b, a);
    }

private:
    Functor f_;
};

template <class Functor>
class ReverseUnaryFunctor {
public:
    ReverseUnaryFunctor() = delete;
    ReverseUnaryFunctor(Functor f) : f_(f) {
    }

    template <typename T>
    inline bool operator()(const T& a) const {
        return !f_(a);
    }

private:
    Functor f_;
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}
