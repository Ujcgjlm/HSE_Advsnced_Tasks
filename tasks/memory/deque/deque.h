#pragma once

#include <cstddef>
#include <initializer_list>
#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>

class Deque {
public:
    Deque() : buffer_allocated_size_(2) {
        Allocate();
    }

    Deque(const Deque& rhs)
        : buffer_allocated_size_(std::max(1ul, (rhs.deque_size_ + block_size_ - 1) / block_size_)) {
        Allocate();
        rhs.Copy(buffer_);

        buffer_end_ = deque_size_ / block_size_;
        deque_size_ = rhs.deque_size_;
        deque_end_ = deque_size_ % block_size_;

        if (buffer_allocated_size_ * block_size_ == deque_size_) {
            buffer_end_ = 0;
        }
    }

    Deque(Deque&& rhs) {
        Swap(rhs);
    }

    explicit Deque(size_t size)
        : buffer_allocated_size_(std::max(1ul, (size + block_size_ - 1) / block_size_)) {
        Allocate();
        for (size_t i = 0; i < size; ++i) {
            PushBack(0);
        }
    }

    Deque(std::initializer_list<int> list)
        : buffer_allocated_size_(std::max(1ul, (list.size() + block_size_ - 1) / block_size_)) {
        Allocate();
        for (auto i : list) {
            PushBack(i);
        }
    }

    Deque& operator=(Deque rhs) {
        Swap(rhs);
        return *this;
    }

    void Swap(Deque& rhs) {
        std::swap(buffer_allocated_size_, rhs.buffer_allocated_size_);
        std::swap(buffer_begin_, rhs.buffer_begin_);
        std::swap(buffer_end_, rhs.buffer_end_);
        std::swap(deque_size_, rhs.deque_size_);
        std::swap(deque_begin_, rhs.deque_begin_);
        std::swap(deque_end_, rhs.deque_end_);
        std::swap(buffer_, rhs.buffer_);
    }

    void PushBack(int value) {
        MayBeRelocate();

        buffer_[buffer_end_][deque_end_] = value;
        GoRight(buffer_end_, deque_end_);
        ++deque_size_;
    }

    void PopBack() {
        GoLeft(buffer_end_, deque_end_);
        --deque_size_;
    }

    void PushFront(int value) {
        MayBeRelocate();

        GoLeft(buffer_begin_, deque_begin_);
        buffer_[buffer_begin_][deque_begin_] = value;
        ++deque_size_;
    }

    void PopFront() {
        GoRight(buffer_begin_, deque_begin_);
        --deque_size_;
    }

    int& operator[](size_t ind) {
        return buffer_[(buffer_begin_ + (deque_begin_ + ind) / block_size_) %
                       buffer_allocated_size_][(deque_begin_ + ind) % block_size_];
    }

    int operator[](size_t ind) const {
        return buffer_[(buffer_begin_ + (deque_begin_ + ind) / block_size_) %
                       buffer_allocated_size_][(deque_begin_ + ind) % block_size_];
    }

    size_t Size() const {
        return deque_size_;
    }

    void Clear() {
        deque_size_ = 0;
        deque_begin_ = deque_end_;
        buffer_begin_ = buffer_end_;
    }

private: 
    const size_t block_size_ = 512 / sizeof(int);

    std::shared_ptr<std::shared_ptr<int[]>[]> buffer_ = nullptr;

    size_t buffer_allocated_size_ = 1;
    // [*; *)
    size_t buffer_begin_ = 0;
    size_t buffer_end_ = 0;

    size_t deque_size_ = 0;
    // [*; *)
    size_t deque_begin_ = 0;
    size_t deque_end_ = 0;

    void GoRight(size_t& block, size_t& pos) const {
        if (pos + 1 != block_size_) {
            ++pos;
            return;
        }
        pos = 0;
        if (block + 1 != buffer_allocated_size_) {
            ++block;
            return;
        }
        block = 0;
    }

    void GoLeft(size_t& block, size_t& pos) const {
        if (pos != 0) {
            --pos;
            return;
        }
        pos = block_size_ - 1;
        if (block != 0) {
            --block;
            return;
        }
        block = buffer_allocated_size_ - 1;
    }

    void Copy(const std::shared_ptr<std::shared_ptr<int[]>[]>& empty_buffer) const {
        size_t buffer_begin = buffer_begin_;
        size_t deque_begin = deque_begin_;
        size_t empty_buffer_begin = 0;
        size_t empty_deque_begin = 0;

        size_t size = 0;
        while (size < deque_size_) {
            empty_buffer[empty_buffer_begin][empty_deque_begin] =
                buffer_[buffer_begin][deque_begin];
            GoRight(buffer_begin, deque_begin);
            GoRight(empty_buffer_begin, empty_deque_begin);
            ++size;
        }
    }

    void Allocate() {
        buffer_ = std::shared_ptr<std::shared_ptr<int[]>[]>(
            new std::shared_ptr<int[]>[buffer_allocated_size_]);
        for (size_t i = 0; i < buffer_allocated_size_; ++i) {
            buffer_[i] = std::shared_ptr<int[]>(new int[block_size_]);
            for (size_t j = 0; j < block_size_; ++j) {
                buffer_[i][j] = 0;
            }
        }
    }

    void MayBeRelocate() {
        if (deque_size_ != buffer_allocated_size_ * block_size_) {
            return;
        }

        std::shared_ptr<std::shared_ptr<int[]>[]> empty_buffer(
            new std::shared_ptr<int[]>[buffer_allocated_size_ * 2]);
        for (size_t i = 0; i < buffer_allocated_size_ * 2; ++i) {
            empty_buffer[i] = std::shared_ptr<int[]>(new int[block_size_]);
        }

        size_t i_new = 0;
        size_t i_cur = buffer_begin_;

        do {
            empty_buffer[i_new].swap(buffer_[i_cur]);
            ++i_new;
            ++i_cur;
            if (i_cur == buffer_allocated_size_) {
                i_cur = 0;
            }
        } while (i_cur != buffer_end_);

        deque_begin_ = 0;
        buffer_begin_ = 0;
        deque_end_ = deque_size_ % block_size_;
        buffer_end_ = buffer_allocated_size_;

        buffer_allocated_size_ *= 2;

        buffer_.swap(empty_buffer);
    }
};
