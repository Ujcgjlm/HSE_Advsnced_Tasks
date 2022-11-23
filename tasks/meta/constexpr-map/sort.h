#pragma once

#include <constexpr_map.h>

#include <algorithm>
#include <type_traits>

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    if (!std::is_integral_v<K>) {
        for (size_t i = 0; i < map.Size(); ++i) {
            for (size_t j = i + 1; j < map.Size(); ++j) {
                if (!(map.GetByIndex(i).first < map.GetByIndex(j).first)) {
                    std::swap(map.GetByIndex(i), map.GetByIndex(j));
                }
            }
        }
    } else {
        for (size_t i = 0; i < map.Size(); ++i) {
            for (size_t j = i + 1; j < map.Size(); ++j) {
                if (map.GetByIndex(i).first < map.GetByIndex(j).first) {
                    std::swap(map.GetByIndex(i), map.GetByIndex(j));
                }
            }
        }
    }

    return map;
}
