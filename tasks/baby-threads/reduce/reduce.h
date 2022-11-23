#pragma once

#include <cstdint>
#include <thread>
#include <vector>

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    std::vector<std::thread> threads_pool;
    size_t threads_count = 1;
    std::atomic<T> ans(initial_value);
    uint64_t n = last - first;
    uint64_t step = n;

    if (n > 1000 && std::thread::hardware_concurrency() > 2) {
        threads_count += std::thread::hardware_concurrency() - 1;
        step /= threads_count;
    }

    for (uint64_t thread_num = 0; thread_num < threads_count; ++thread_num) {
        threads_pool.emplace_back(
            [&](RandomAccessIterator first, RandomAccessIterator last) {
                auto cur_value{initial_value};
                while (first != last) {
                    cur_value = func(cur_value, *first++);
                }
                ans = func(ans, cur_value);
                return;
            },
            first + thread_num * step, first + std::min((thread_num + 1) * step, n));
    }

    for (auto& thread : threads_pool) {
        thread.join();
    }

    return ans;
}
