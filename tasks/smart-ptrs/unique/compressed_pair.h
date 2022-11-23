#pragma once

#include <type_traits>
#include <utility>

template <typename F, typename S,
          bool is_same_types =
              (std::is_same_v<F, S> || std::is_base_of_v<F, S> || std::is_base_of_v<S, F>),
          bool is_internalF = (std::is_empty_v<F> && !std::is_final_v<F>),
          bool is_internalS = (std::is_empty_v<S> && !std::is_final_v<S>)>
class CompressedPair;

template <typename F, typename S, bool third>
class CompressedPair<S, F, third, true, false> : private CompressedPair<F, S, third, false, true> {
};

template <typename F, typename S>
class CompressedPair<S, F, true, true, true> : private CompressedPair<F, S, true, false, true> {};

template <typename F, typename S, bool third>
class CompressedPair<F, S, third, false, false> {
public:
    CompressedPair() = default;

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : first_(std::forward<U1>(first)), second_(std::forward<U2>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_ = F();
    S second_ = S();
};

template <typename F, typename S, bool third>
class CompressedPair<F, S, third, false, true> : private S {
public:
    CompressedPair() = default;

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : S(std::forward<U2>(second)), first_(std::forward<U1>(first)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return *this;
    };

    const S& GetSecond() const {
        return *this;
    };

private:
    F first_ = F();
};

template <typename F, typename S>
class CompressedPair<F, S, false, true, true> : private F, private S {
public:
    CompressedPair() = default;

    template <typename U1, typename U2>
    CompressedPair(U1&& first, U2&& second)
        : F(std::forward<U1>(first)), S(std::forward<U2>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return *this;
    };

    const S& GetSecond() const {
        return *this;
    };
};
