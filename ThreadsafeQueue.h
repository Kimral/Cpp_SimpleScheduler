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
    ThreadsafeQueue(ThreadsafeQueue&& other) noexcept;
    ThreadsafeQueue& operator=(ThreadsafeQueue&& other) noexcept;

    bool empty();
    bool try_push(Task& t);
    void push(Task& t);
    bool try_pop(Task& t);

private:
    std::queue<Task> queue_;
    std::mutex mutex_;
};