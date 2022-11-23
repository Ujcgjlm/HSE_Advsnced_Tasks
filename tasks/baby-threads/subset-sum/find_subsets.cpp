#include "find_subsets.h"
#include <bitset>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

Subsets FindEqualSumSubsets(const std::vector<int64_t>& data) {
    std::unordered_map<int64_t, size_t> map;

    std::mutex find_ans;
    Subsets ans{{}, {}, false};

    size_t first_part_n = std::max(0l, static_cast<int64_t>(data.size() / 2) - 2);
    size_t second_part_n = data.size() - first_part_n;
    size_t number_of_iterations = 1;
    for (size_t i = 0; i < first_part_n; ++i) {
        number_of_iterations *= 3ull;
    }

    map.reserve(number_of_iterations);

    for (size_t iteration = 0; iteration < number_of_iterations; ++iteration) {
        size_t mask = iteration;

        size_t i = 0;
        int64_t cursum = 0;
        short cur_type = 0;
        bool has_one = false;
        bool has_two = false;
        while (mask) {
            cur_type = mask % 3ull;
            mask /= 3ull;
            if (cur_type == 1) {
                cursum += data[i];
                has_one = true;
            }
            if (cur_type == 2) {
                cursum -= data[i];
                has_two = true;
            }
            ++i;
        }

        if (has_one && has_two && cursum == 0) {
            i = 0;
            ans.exists = true;
            while (iteration) {
                cur_type = iteration % 3ull;
                iteration /= 3ull;
                if (cur_type == 1) {
                    ans.first_indices.push_back(i);
                }
                if (cur_type == 2) {
                    ans.second_indices.push_back(i);
                }
                ++i;
            }
            return ans;
        }

        map[-cursum] = iteration;
    }

    number_of_iterations = 1;
    for (size_t i = 0; i < second_part_n; ++i) {
        number_of_iterations *= 3ull;
    }

    std::vector<std::thread> threads_pool;
    size_t threads_count = 1;
    uint64_t step = number_of_iterations;

    if (number_of_iterations > 10000 && std::thread::hardware_concurrency() > 2) {
        threads_count += std::thread::hardware_concurrency();
        step = (number_of_iterations + threads_count - 1) / threads_count;
    }

    for (uint64_t thread_num = 0; thread_num < threads_count; ++thread_num) {
        threads_pool.emplace_back(
            [&](size_t start, size_t end) {
                size_t mask;
                size_t i;
                int64_t cursum;
                short cur_type;
                bool has_one;
                bool has_two;
                for (size_t iteration = start; iteration < end; ++iteration) {
                    mask = iteration;

                    i = 0;
                    cursum = 0;
                    cur_type = 0;
                    has_one = false;
                    has_two = false;
                    while (mask) {
                        cur_type = mask % 3ull;
                        mask /= 3ull;
                        if (cur_type == 1) {
                            cursum += data[first_part_n + i];
                            has_one = true;
                        }
                        if (cur_type == 2) {
                            cursum -= data[first_part_n + i];
                            has_two = true;
                        }
                        ++i;
                    }

                    if (has_one && has_two && cursum == 0) {
                        i = 0;
                        std::lock_guard<std::mutex> g(find_ans);
                        if (ans.exists) {
                            return;
                        }
                        ans.exists = true;
                        while (iteration) {
                            cur_type = iteration % 3ull;
                            iteration /= 3ull;
                            if (cur_type == 1) {
                                ans.first_indices.push_back(first_part_n + i);
                            }
                            if (cur_type == 2) {
                                ans.second_indices.push_back(first_part_n + i);
                            }
                            ++i;
                        }
                        return;
                    }
                    if (map.contains(cursum)) {
                        mask = map[cursum];
                        while (mask) {
                            cur_type = mask % 3ull;
                            mask /= 3ull;
                            if (cur_type == 1) {
                                has_one = true;
                            }
                            if (cur_type == 2) {
                                has_two = true;
                            }
                        }

                        if (has_one && has_two) {
                            i = 0;
                            std::lock_guard<std::mutex> g(find_ans);
                            if (ans.exists) {
                                return;
                            }
                            ans.exists = true;
                            while (iteration) {
                                cur_type = iteration % 3ull;
                                iteration /= 3ull;
                                if (cur_type == 1) {
                                    ans.first_indices.push_back(first_part_n + i);
                                }
                                if (cur_type == 2) {
                                    ans.second_indices.push_back(first_part_n + i);
                                }
                                ++i;
                            }
                            i = 0;
                            iteration = map[cursum];
                            while (iteration) {
                                cur_type = iteration % 3ull;
                                iteration /= 3ull;
                                if (cur_type == 1) {
                                    ans.first_indices.push_back(i);
                                }
                                if (cur_type == 2) {
                                    ans.second_indices.push_back(i);
                                }
                                ++i;
                            }
                            return;
                        }
                    }
                }
            },
            step * thread_num, std::min(number_of_iterations, step * (thread_num + 1)));
    }

    for (auto& thread : threads_pool) {
        thread.join();
    }

    return ans;
}
