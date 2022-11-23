#pragma once

#include <algorithm>
#include <cstdlib>

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const {
        return front_link_;
    }

    void Unlink() {
        if (IsLinked()) {
            LinkEachOther(front_link_, back_link_);
            front_link_ = back_link_ = nullptr;
        }
    }

    // Must unlink element from list
    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

private:
    template <class T>
    friend class List;

    ListHook* front_link_ = nullptr;
    ListHook* back_link_ = nullptr;

    // that helper function might be useful
    void LinkBefore(ListHook* other) {
        LinkEachOther(other, back_link_);
        LinkEachOther(this, other);
    };

    static void LinkEachOther(ListHook* first, ListHook* second) {
        first->back_link_ = second;
        second->front_link_ = first;
    }
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator(T* hook) {
            current_ = hook;
        }

        Iterator& operator++() {
            current_ = reinterpret_cast<T*>(current_->front_link_);
            return *this;
        }
        Iterator operator++(int) {
            auto prev = *this;
            ++*this;
            return prev;
        }

        T& operator*() const {
            return *current_;
        }
        T* operator->() const {
            return current_;
        }

        bool operator==(const Iterator& rhs) const {
            return current_ == rhs.current_;
        }
        bool operator!=(const Iterator& rhs) const {
            return current_ != rhs.current_;
        }

    private:
        T* current_;
    };

    List() {
        dummy_ = new ListHook();
        ListHook::LinkEachOther(dummy_, dummy_);
    }
    List(const List&) = delete;
    List(List&& other) {
        dummy_ = new ListHook();
        ListHook::LinkEachOther(dummy_, dummy_);
        std::swap(dummy_, other.dummy_);
    }

    // must unlink all elements from list
    ~List() {
        UnlinkAll();
        delete dummy_;
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        UnlinkAll();
        delete dummy_;
        dummy_ = new ListHook();
        ListHook::LinkEachOther(dummy_, dummy_);
        std::swap(dummy_, other.dummy_);
        return *this;
    }

    void UnlinkAll() {
        while (Size()) {
            dummy_->front_link_->Unlink();
        }
    }

    bool IsEmpty() const {
        return Begin() == End();
    }
    // that method is allowed to be O(n)
    size_t Size() const {
        size_t size = 0;
        for (auto i = Begin(); i != End(); ++i) {
            ++size;
        }
        return size;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        dummy_->LinkBefore(elem);
    }
    void PushFront(T* elem) {
        dummy_->front_link_->LinkBefore(elem);
    }

    T& Front() {
        return *reinterpret_cast<T*>(dummy_->front_link_);
    }
    const T& Front() const {
        return *reinterpret_cast<T*>(dummy_->front_link_);
    }

    T& Back() {
        return *reinterpret_cast<T*>(dummy_->back_link_);
    }
    const T& Back() const {
        return *reinterpret_cast<T*>(dummy_->back_link_);
    }

    void PopBack() {
        auto back_link = dummy_->back_link_;
        ListHook::LinkEachOther(dummy_, back_link->back_link_);
        back_link->Unlink();
    }
    void PopFront() {
        auto front_link = dummy_->front_link_;
        ListHook::LinkEachOther(dummy_, front_link->front_link_);
        front_link->Unlink();
    }

    Iterator Begin() const {
        return ++Iterator(reinterpret_cast<T*>(dummy_));
    }
    Iterator End() const {
        return Iterator(reinterpret_cast<T*>(dummy_));
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

private:
    ListHook* dummy_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
