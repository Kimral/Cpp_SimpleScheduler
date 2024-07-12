#pragma once

#include <vector>
#include <thread>
#include <deque>
#include <functional>
#include <mutex>

template <class TaskFunction>
class Sheduler {
public:
    Sheduler() = default;
    explicit Sheduler(size_t threads_count) : threads_count_{ threads_count } {}

	void Start() {
		running = true;

        // Изменяем размер вектора потоков до необходимого количества
        threads_.resize(threads_count_);

        std::unique_lock<std::mutex> lock(threads_mutex_);
        for (size_t i = 0; i < threads_count_; ++i) {
            threads_[i] = std::thread([this, i] {
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
                            std::cout << "Thread id: " << i  << " " << std::this_thread::get_id() << std::endl;
                            task();
                        }
                        catch (...) {
                            std::cerr << "Exception caught during task execution.\n";
                        }
                    }
                }
            });
        }
	}

    ~Sheduler() {
        for(;;) {
            if (tasks_.empty())
                break;
        }

        running = false;
        for (auto& t : threads_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

	void Stop() {
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

	std::mutex tasks_mutex_;
	std::deque<TaskFunction> tasks_;

	std::mutex threads_mutex_;
	std::vector<std::thread> threads_;
};