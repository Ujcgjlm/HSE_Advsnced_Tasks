#pragma once

#include <algorithm>
#include <type_traits>
#include <utility>

template <class Iterator>
class IteratorRange {
public:
    IteratorRange() = delete;
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    }

    Iterator begin() const {  // NOLINT
        return begin_;
    }

    Iterator end() const {  // NOLINT
        return end_;
    }

private:
    Iterator begin_, end_;
};

template <typename TFirst, typename TSecond>
class RangeIterator {
public:
    RangeIterator() = delete;
    RangeIterator(const TFirst& begin, const TSecond& end, size_t step)
        : step_(step), cur_(begin), end_(end) {
    }

    RangeIterator(const RangeIterator& another)
        : step_(another.step_), cur_(another.cur_), end_(another.end_) {
    }

    inline RangeIterator& operator=(const RangeIterator& another) {
        step_ = another.step_;
        cur_ = another.cur_;
        end_ = another.end_;
        return *this;
    }

    inline bool operator!=(const RangeIterator& a) const {
        if (cur_ >= end_ && a.cur_ >= a.end_) {
            return false;
        }
        return cur_ != a.cur_;
    }

    inline RangeIterator operator++() {
        cur_ += step_;
        return *this;
    }

    inline TFirst operator*() const {
        return cur_;
    }

private:
    size_t step_ = 1;
    TFirst cur_ = 0;
    TSecond end_;
};

template <typename TFirst, typename TSecond>
auto Range(TFirst begin, TSecond end, size_t step) {
    return IteratorRange(RangeIterator<TFirst, TSecond>(begin, end, step),
                         RangeIterator(end, end, step));
}

template <typename TFirst, typename TSecond>
auto Range(TFirst begin, TSecond end) {
    return IteratorRange(RangeIterator<TFirst, TSecond>(begin, end, 1), RangeIterator(end, end, 1));
}

template <typename TSecond>
auto Range(TSecond end) {
    return IteratorRange(RangeIterator<size_t, TSecond>(0, end, 1),
                         RangeIterator<size_t, TSecond>(end, end, 1));
}

template <typename TFirst, typename TSecond,
          typename TFirstIter = decltype(std::declval<const TFirst>().begin()),
          typename TSecondIter = decltype(std::declval<const TSecond>().begin())>
class ZipIterator {
public:
    ZipIterator() = delete;
    ZipIterator(const TFirst& first, const TSecond& second)
        : first_cur_(first.begin()),
          first_end_(first.end()),
          second_cur_(second.begin()),
          second_end_(second.end()),
          first_(first),
          second_(second) {
    }

    ZipIterator(const ZipIterator& another)
        : first_cur_(another.first_cur_),
          first_end_(another.first_end_),
          second_cur_(another.second_cur_),
          second_end_(another.second_end_),
          first_(another.first_),
          second_(another.second_) {
    }

    inline bool operator!=(const ZipIterator& a) const {
        if ((!(first_cur_ != first_end_) || !(second_cur_ != second_end_)) &&
            (!(a.first_cur_ != a.first_end_) || !(a.second_cur_ != a.second_end_))) {
            return false;
        }
        return (first_cur_ != a.first_cur_) || (second_cur_ != a.second_cur_);
    }

    inline ZipIterator operator++() {
        ++first_cur_;
        ++second_cur_;
        return *this;
    }

    inline auto operator*() const {
        return std::make_pair(*first_cur_, *second_cur_);
    }

    ZipIterator Begin() const {
        return *this;
    }

    ZipIterator End() const {
        auto zip_iter_end = ZipIterator(first_, second_);
        zip_iter_end.first_cur_ = zip_iter_end.first_end_;
        zip_iter_end.second_cur_ = zip_iter_end.second_end_;
        return zip_iter_end;
    }

private:
    TFirstIter first_cur_;
    TFirstIter first_end_;
    TSecondIter second_cur_;
    TSecondIter second_end_;
    const TFirst& first_;
    const TSecond& second_;
};

template <typename TFirst, typename TSecond>
auto Zip(const TFirst& first, const TSecond& second) {
    auto iter = ZipIterator(first, second);
    return IteratorRange(iter.Begin(), iter.End());
}

template <typename T, typename TIter = decltype(std::declval<const T>().begin())>
class GroupIterator {
public:
    GroupIterator() = delete;
    GroupIterator(const T& first) : first_(first), cur_(first.begin()), end_(first.end()) {
        next_ = GetEndOfGroup(cur_);
    }

    GroupIterator(const GroupIterator& another)
        : cur_(another.cur_), end_(another.end_), next_(another.next_), first_(another.first_) {
    }

    inline bool operator!=(const GroupIterator& a) const {
        return cur_ != a.cur_;
    }

    inline GroupIterator operator++() {
        cur_ = next_;
        next_ = GetEndOfGroup(cur_);
        return *this;
    }

    inline T operator*() const {
        return T(cur_, next_);
    }

    GroupIterator Begin() const {
        return *this;
    }

    GroupIterator End() const {
        auto group_iter_end = GroupIterator(first_);
        group_iter_end.cur_ = group_iter_end.end_;
        group_iter_end.next_ = group_iter_end.end_;
        return group_iter_end;
    }

private:
    const T& first_;
    TIter cur_;
    TIter next_;
    TIter end_;

    TIter GetEndOfGroup(TIter cur) const {
        if (!(cur != end_)) {
            return cur;
        }

        auto cur_val = *cur;
        while (cur != end_ && cur_val == *cur) {
            ++cur;
        }
        return cur;
    }
};

template <typename T>
auto Group(const T& container) {
    auto iter = GroupIterator(container);
    return IteratorRange(iter.Begin(), iter.End());
}
