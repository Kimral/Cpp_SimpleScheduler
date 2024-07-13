#pragma once

#include <vector>
#include <thread>
#include <deque>
#include <functional>
#include <mutex>

class SchedulerInterface {
public:
    SchedulerInterface() {};
    virtual ~SchedulerInterface() {};

    virtual void Start() = 0;
    virtual void Stop() = 0;
};

template <class TaskFunction>
class Scheduler : public SchedulerInterface {
public:
    Scheduler() = default;
    explicit Scheduler(size_t threads_count) : threads_count_{ threads_count } {}

    ~Scheduler() override {
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        deque_open = false;
        dequeCondition.wait(lock, [this] {
            return deque_open == false && tasks_.empty();
        });

        running = false;
        for (auto& t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

	void Start() override {
		running = true;

        std::unique_lock<std::mutex> lock(threads_mutex_);
        while (threads_count_--) {
            threads_.emplace_back([this] {
                while (running) {
                    TaskFunction task;
                    {
                        std::unique_lock<std::mutex> lock(tasks_mutex_);
                        taskCondition.wait(lock, [this] { return !tasks_.empty() || !running; });
                        if (!tasks_.empty()) {
                            task = std::move(tasks_.front());
                            tasks_.pop_front();
                        }
                    }
                    if (task) {
                        try {
                            std::cout << "Thread id: " << std::this_thread::get_id() << std::endl;
                            task();
                            dequeCondition.notify_all();
                        }
                        catch (...) {
                            std::cerr << "Exception caught during task execution.\n";
                        }
                    }
                }
            });
        }
	}

	void Stop() override {
		running = false;
	}

    void AddTask(TaskFunction task) {
        if (!deque_open)
            return;
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        tasks_.push_back(task);
        taskCondition.notify_one();
    }
	
private:
	bool running = false;
    bool deque_open = true;
    size_t threads_count_ = 0;
    std::condition_variable taskCondition;
    std::condition_variable dequeCondition;

	std::mutex tasks_mutex_;
	std::deque<TaskFunction> tasks_;

	std::mutex threads_mutex_;
	std::vector<std::thread> threads_;
};