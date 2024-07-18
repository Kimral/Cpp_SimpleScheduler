#pragma once

#include "ThreadsafeQueue.h"

class ThreadState {
public:
	ThreadState() = default;

    ~ThreadState() {
        finish();
        if (thread_.joinable())
            thread_.join();
    }

    void AwaitNonEmptyState() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() {
            return finish_ || !queue_.empty();
        });
    }

	void operator()() {
        Task task;
        for (;;) {
            if (queue_.try_pop(task)) {
                try {
                    task();
                    continue;
                }
                catch (...) {
                    std::cerr << "Exception caught during task execution.\n";
                }
            }
            else if (finish_.load()) {
                break;
            }
            AwaitNonEmptyState();
        }
	}

    void finish() {
        finish_.store(true);
        condition_.notify_one();
    }

    bool try_push(Task& t) {
        auto result = queue_.try_push(t);
        if (result) {
            condition_.notify_one();
        }
        return result;
    }

    void push(Task& t) {
        queue_.push(t);
        condition_.notify_one();
    }

public:
    std::atomic<bool> finish_{ false };
    std::mutex mutex_;
	std::condition_variable condition_;
	ThreadsafeQueue queue_;
	std::thread thread_;
};
