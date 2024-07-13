#pragma once

#include <memory>
#include <mutex>
#include <queue>

#include "common.h"

class ThreadsafeQueue {
public:
    ThreadsafeQueue() = default;
    ThreadsafeQueue(const ThreadsafeQueue&) {
    
    };

    void AwaitEmptyState() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition.wait(lock, [this] {
            return queue_.empty();
        });
    }

    ThreadsafeQueue& operator=(const ThreadsafeQueue& other) {
        ThreadsafeQueue result;
        result.queue_ = other.queue_;
        return result;
    };

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool try_push(Task& t)
    {
        std::unique_lock lock{ mutex_, std::try_to_lock };
        if (!lock)
            return false;

        queue_.emplace(std::move(t));
        return true;
    }

    void push(Task& t)
    {
        std::lock_guard lock{ mutex_ };
        queue_.emplace(std::move(t));
    }

    bool try_pop(Task& t)
    {
        std::lock_guard lock{ mutex_ };
        if (queue_.empty())
            return false;

        t = std::move(queue_.front());
        queue_.pop();
        condition.notify_one();
        return true;
    }

private:
    std::condition_variable condition;
    std::queue<Task> queue_;
    std::mutex mutex_;
};