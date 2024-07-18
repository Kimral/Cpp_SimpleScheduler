#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include "common.h"

class ThreadsafeQueue {
public:
    ThreadsafeQueue() = default;
    ThreadsafeQueue(const ThreadsafeQueue& other) = delete;
    ThreadsafeQueue& operator=(const ThreadsafeQueue& other) = delete;

    ThreadsafeQueue(ThreadsafeQueue&& other) noexcept {
        queue_.swap(other.queue_);
    };

    ThreadsafeQueue& operator=(ThreadsafeQueue&& other) noexcept {
        queue_.swap(other.queue_);
        return *this;
    };  

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool try_push(Task& t) {
        std::unique_lock lock{ mutex_, std::try_to_lock };
        if (!lock)
            return false;

        queue_.emplace(std::move(t));
        return true;
    }

    void push(Task& t) {
        std::lock_guard lock{ mutex_ };
        queue_.emplace(std::move(t));
    }

    bool try_pop(Task& t) {
        std::lock_guard lock{ mutex_ };
        if (queue_.empty())
            return false;

        t = std::move(queue_.front());
        queue_.pop();
        return true;
    }

private:
    std::queue<Task> queue_;
    std::mutex mutex_;
};