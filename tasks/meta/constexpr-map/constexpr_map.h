#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;
    constexpr ~ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (size_t i = 0; i < size_; ++i) {
            if (key_value_storage_[i].first == key) {
                return key_value_storage_[i].second;
            }
        }

        if (size_ == MaxSize) {
            throw std::out_of_range("");
        }

        key_value_storage_[size_].first = key;
        return key_value_storage_[size_++].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (key_value_storage_[i].first == key) {
                return key_value_storage_[i].second;
            }
        }
        throw std::logic_error("Key not exists");
    }

    constexpr bool Erase(const K& key) {
        for (size_t i = 0; i < size_; ++i) {
            if (key_value_storage_[i].first == key) {
                for (size_t j = i + 1; j < size_; ++j) {
                    std::swap(key_value_storage_[j], key_value_storage_[j - 1]);
                }
                --size_;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (key_value_storage_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return size_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return key_value_storage_[pos];
    }   

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return key_value_storage_[pos];
    }

private:
    std::array<std::pair<K, V>, MaxSize> key_value_storage_;
    size_t size_ = 0;
};
