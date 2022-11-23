#pragma once

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>

namespace {
const size_t kBranchingFactor = 32;

template <typename T>
class Node {
public:
    virtual std::shared_ptr<Node<T>> Set(size_t index, size_t allocated_size, const T& value) = 0;
    virtual const T& Get(size_t index, size_t allocated_size) = 0;
};

template <typename T>
class NodeValue;

template <typename T>
class NodePtrs : public Node<T> {
public:
    NodePtrs() = default;

    const T& Get(size_t index, size_t allocated_size) {
        auto cur_index = (index % allocated_size) / (allocated_size / kBranchingFactor);
        return ptrs_[cur_index]->Get(index, allocated_size / kBranchingFactor);
    }

    std::shared_ptr<Node<T>> Set(size_t index, size_t allocated_size, const T& value) {
        auto cur_index = (index % allocated_size) / (allocated_size / kBranchingFactor);
        auto buf = std::make_shared<NodePtrs<T>>(*this);
        if (!buf->ptrs_[cur_index]) {
            if (allocated_size <= kBranchingFactor * kBranchingFactor) {
                buf->ptrs_[cur_index] = std::make_shared<NodeValue<T>>();
            } else {
                buf->ptrs_[cur_index] = std::make_shared<NodePtrs<T>>();
            }
        }
        buf->ptrs_[cur_index] =
            buf->ptrs_[cur_index]->Set(index, allocated_size / kBranchingFactor, value);
        return buf;
    }

    void SetRootFromThis(std::shared_ptr<Node<T>> node) {
        ptrs_[0] = node;
    }

private:
    std::shared_ptr<Node<T>> ptrs_[kBranchingFactor];
};

template <typename T>
class NodeValue : public Node<T> {
public:
    NodeValue() = default;

    const T& Get(size_t index, size_t allocated_size) {
        auto cur_index = (index % allocated_size) / (allocated_size / kBranchingFactor);
        return *ptrs_[cur_index];
    }

    std::shared_ptr<Node<T>> Set(size_t index, size_t allocated_size, const T& value) {
        auto cur_index = (index % allocated_size) / (allocated_size / kBranchingFactor);
        auto buf = std::make_shared<NodeValue<T>>(*this);
        buf->ptrs_[cur_index] = std::make_shared<T>(value);
        return buf;
    }

private:
    std::shared_ptr<T> ptrs_[kBranchingFactor];
};
}  // namespace

template <class T>
class ImmutableVector {
public:
    ImmutableVector() {
    }

    explicit ImmutableVector(size_t count, const T& value = T()) {
        for (size_t i = 0; i < count; ++i) {
            *this = PushBack(value);
        }
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        for (auto it = first; it != last; ++it) {
            *this = PushBack(*it);
        }
    }

    ImmutableVector(std::initializer_list<T> l) : ImmutableVector<T>(l.begin(), l.end()) {
    }

    ImmutableVector Set(size_t index, const T& value) {
        return ImmutableVector<T>(root_->Set(index, allocated_size_, value), size_,
                                  allocated_size_);
    }

    const T& Get(size_t index) const {
        return root_->Get(index, allocated_size_);
    }

    ImmutableVector PushBack(const T& value) {
        auto buf = *this;
        if (buf.size_ == buf.allocated_size_) {
            auto buf_root = std::make_shared<NodePtrs<T>>();
            buf_root->SetRootFromThis(root_);
            buf.root_ = buf_root;

            buf.allocated_size_ *= kBranchingFactor;
        }
        buf.root_ = buf.root_->Set(buf.size_, buf.allocated_size_, value);
        ++buf.size_;
        return buf;
    }

    ImmutableVector PopBack() {
        auto buf = *this;
        --buf.size_;
        return buf;
    }

    size_t Size() const {
        return size_;
    }

private:
    std::shared_ptr<Node<T>> root_ = std::make_shared<NodeValue<T>>();
    size_t size_ = 0;
    size_t allocated_size_ = kBranchingFactor;

    ImmutableVector(std::shared_ptr<Node<T>> root, size_t size, size_t allocated_size)
        : root_(root), size_(size), allocated_size_(allocated_size) {
    }
};
