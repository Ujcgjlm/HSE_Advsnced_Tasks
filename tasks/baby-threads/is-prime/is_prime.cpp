#include "is_prime.h"
#include <sys/types.h>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

void Work(uint64_t x, uint64_t from, uint64_t to, std::atomic<bool>& ans) {
    uint64_t i = std::max(from, 2ul);
    while (i <= to) {
        if (!ans) {
            return;
        }
        for (uint64_t next = std::min(to, i + 100000); i <= next; ++i) {
            if (!(x % i)) {
                ans = false;
                return;
            }
        }
    }
}

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }

    std::vector<std::thread> threads_pool;
    size_t threads_count = 1;
    std::atomic<bool> ans = true;
    uint64_t step = sqrt(x);

    if (x > 1000000 && std::thread::hardware_concurrency() > 2) {
        threads_count += std::thread::hardware_concurrency() - 1;
        step /= threads_count;

        for (uint64_t i = 0; i < threads_count - 1; ++i) {
            threads_pool.emplace_back(Work, x, i * step, (i + 1) * step, std::ref(ans));
        }
    }

    threads_pool.emplace_back(Work, x, (threads_count - 1) * step, sqrt(x), std::ref(ans));

    for (auto& thread : threads_pool) {
        thread.join();
    }

    return ans;
}
