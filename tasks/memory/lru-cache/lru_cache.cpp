#include "lru_cache.h"
#include <utility>

LruCache::LruCache(size_t max_size) : max_size_(max_size) {
}

void LruCache::Set(const std::string& key, const std::string& value) {
    auto map_iter = map_.find(key);

    if (map_iter != map_.end()) {
        MoveIterToTheEnd(map_iter);
        lru_list_.back() = std::tie(key, value);
    } else {
        lru_list_.emplace_back(key, value);
        map_.insert({key, std::prev(lru_list_.end())});
    }

    if (lru_list_.size() > max_size_) {
        map_.erase(std::get<0>(lru_list_.front()));
        lru_list_.pop_front();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    auto map_iter = map_.find(key);

    if (map_iter != map_.end()) {
        MoveIterToTheEnd(map_iter);
        *value = std::get<1>(*map_iter->second);
        return true;
    }
    return false;
}

void LruCache::MoveIterToTheEnd(const TLruMap::iterator& map_iter) {
    auto list_iter = map_iter->second;
    lru_list_.push_back(*list_iter);
    map_iter->second = std::prev(lru_list_.end());
    lru_list_.erase(list_iter);
}
