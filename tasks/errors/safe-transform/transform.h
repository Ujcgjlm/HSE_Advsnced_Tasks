#pragma once

#include <iostream>
#include <list>

template <typename Iterator,
          typename ValueType = std::remove_cvref_t<decltype(*std::declval<const Iterator>())>>
void Unstash(const std::list<std::pair<Iterator, ValueType>>& stash) {
    for (auto& [it, value] : stash) {
        *it = value;
    }
}

template <typename Iterator, typename Predictor, typename Functor,
          typename ValueType = std::remove_cvref_t<decltype(*std::declval<const Iterator>())>>
void TransformIf(Iterator begin, Iterator end, Predictor p, Functor f) {
    std::list<std::pair<Iterator, ValueType>> stash;
    for (auto it = begin; it != end; ++it) {
        bool predicate;
        try {
            predicate = p(*it);
        } catch (...) {
            Unstash(stash);
            throw;
        }
        if (predicate) {
            try {
                stash.emplace_back(it, *it);
            } catch (...) {
            }
            try {
                f(*it);
            } catch (...) {
                Unstash(stash);
                throw;
            }
        }
    }
}
