#pragma once

#include <list>
#include <string>
#include <unordered_map>

class LruCache {
public:
    using TKeyValueList = std::list<std::tuple<std::string, std::string>>;
    using TLruMap = std::unordered_map<std::string, TKeyValueList::iterator>;

    LruCache() = delete;
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);
    bool Get(const std::string& key, std::string* value);

private:
    TLruMap map_;
    TKeyValueList lru_list_;

    void MoveIterToTheEnd(const TLruMap::iterator& list_iter);

    const size_t max_size_;
};
