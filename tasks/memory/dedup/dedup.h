#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> duplicated_items;

    for (const auto& i : items) {
        duplicated_items.push_back(std::make_unique<string>(*i));
    }

    return duplicated_items;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::unordered_map<string, std::shared_ptr<string>> map;
    std::vector<std::shared_ptr<string>> de_duplicated_items;

    for (const auto& i : items) {
        auto [iter, _] = map.try_emplace(*i, std::make_shared<string>(*i));
        de_duplicated_items.push_back(iter->second);
    }

    return de_duplicated_items;
}
