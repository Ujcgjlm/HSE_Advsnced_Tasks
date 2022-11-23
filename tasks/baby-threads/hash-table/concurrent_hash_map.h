#pragma once

#include <atomic>
#include <deque>
#include <list>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <utility>

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }

    [[maybe_unused]] ConcurrentHashMap(int expected_size, int expected_threads_count,
                                       const Hash& hasher = Hash())
        : hasher_(hasher) {

        locks_.resize(expected_threads_count);
        allocated_size_ = expected_size + (expected_size % expected_threads_count);
        table_.resize(allocated_size_);
    }

    bool Insert(const K& key, const V& value) {
        auto hash_value = hasher_(key);
        locks_[hash_value % locks_.size()].lock();
        auto& cur_list = table_[hash_value % table_.size()];
        for (auto it = cur_list.begin(); it != cur_list.end(); ++it) {
            if (key == it->first) {
                locks_[hash_value % locks_.size()].unlock();
                return false;
            }
        }

        cur_list.emplace_back(key, value);
        ++size_;

        if (cur_list.size() == 14) {
            locks_[hash_value % locks_.size()].unlock();
            for (auto& mutex : locks_) {
                mutex.lock();
            }

            allocated_size_ = allocated_size_ * 3;
            std::deque<std::list<std::pair<K, V>>> buf(allocated_size_);

            for (auto list : table_) {
                for (auto [key, value] : list) {
                    buf[hasher_(key) % buf.size()].emplace_back(key, value);
                }
            }

            table_ = std::move(buf);

            for (auto& mutex : locks_) {
                mutex.unlock();
            }
        } else {
            locks_[hash_value % locks_.size()].unlock();
        }

        return true;
    }

    bool Erase(const K& key) {
        auto hash_value = hasher_(key);
        std::lock_guard<std::mutex> lock(locks_[hash_value % locks_.size()]);
        auto& cur_list = table_[hash_value % table_.size()];
        for (auto it = cur_list.begin(); it != cur_list.end(); ++it) {
            if (key == it->first) {
                cur_list.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }

    void Clear() {
        for (auto& mutex : locks_) {
            mutex.lock();
        }

        table_.assign(allocated_size_, {});
        size_ = 0;

        for (auto& mutex : locks_) {
            mutex.unlock();
        }
    }

    std::pair<bool, V> Find(const K& key) const {
        auto hash_value = hasher_(key);
        std::lock_guard<std::mutex> lock(locks_[hash_value % locks_.size()]);
        auto cur_list = table_[hash_value % table_.size()];
        for (auto& [cur_key, value] : cur_list) {
            if (key == cur_key) {
                return std::make_pair(true, value);
            }
        }
        return std::make_pair(false, V());
    }

    const V At(const K& key) const {
        auto ans = Find(key);
        if (!ans.first) {
            throw std::out_of_range("At");
        }
        return ans.second;
    }

    size_t Size() const {
        return size_;
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    Hash hasher_;
    std::deque<std::list<std::pair<K, V>>> table_;
    mutable std::deque<std::mutex> locks_;
    std::atomic<size_t> size_ = 0;
    std::atomic<size_t> allocated_size_ = 0;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = 80;
