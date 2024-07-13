#pragma once

#include "ThreadsafeQueue.h"

class Worker {
public:
    Worker() = default;

    Worker(const Worker& other) {
        queue_ = other.queue_;
    };

    ~Worker() {
        queue_.AwaitEmptyState();
        running = false;
        if(thread_.joinable())
            thread_.join();
    }

    Worker& operator=(const Worker&) = delete;

    void Start() {
        running = true;
        thread_ = std::thread{ [this]() {
            std::cout << "Worker started. Thread: " << thread_.get_id() << std::endl;
            while (running) {
                Task task;
                if(queue_.try_pop(task)) {
                    try {
                        //std::cout << " Thread id: " << std::this_thread::get_id() << std::endl;
                        task();
                    }
                    catch (...) {
                        std::cerr << "Exception caught during task execution.\n";
                    }
                }
            }
        }};
    }

    bool try_push(Task& t) {
        return queue_.try_push(t);
    }

    void push(Task& t) {
        queue_.push(t);
    }

private:
    bool running = false;

    std::condition_variable task_condition_;
    ThreadsafeQueue queue_;
    std::thread thread_;
};