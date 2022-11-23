#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <queue>
#include <thread>
#include <utility>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        std::unique_lock<std::mutex> guard(mutex_);
        if (queue_.empty()) {
            condvar_empty_.notify_one();
        } else {
            if (queue_.top().first > at) {
                condvar_waiters_.notify_one();
            }
        }
        queue_.push({at, item});
    }

    T Pop() {
        std::unique_lock<std::mutex> guard(mutex_);

        condvar_empty_.wait(guard, [this]() -> bool { return !queue_.empty(); });

        while (queue_.top().first > Clock::now()) {
            auto time = queue_.top().first;
            condvar_waiters_.wait_until(guard, time);
        }

        T ans = queue_.top().second;
        queue_.pop();

        return ans;
    }

private:
    std::priority_queue<std::pair<TimePoint, T>, std::vector<std::pair<TimePoint, T>>,
                        std::greater<std::pair<TimePoint, T>>>
        queue_;
    std::condition_variable condvar_empty_;
    std::condition_variable condvar_waiters_;
    std::mutex mutex_;
};
