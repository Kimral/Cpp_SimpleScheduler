#pragma once

#include "common.h"
#include "worker.h"

class Scheduler {
public:
    Scheduler() = default;
    Scheduler(size_t workers_count) : workers_{ workers_count } {}
    ~Scheduler() {
        for (auto& worker : workers_) {
            worker.finish();
        }
    }

    void Start() {
        for (auto& worker : workers_) {
            worker.Start();
        }
    }

    void push(Task t)
    {
        for (size_t i = 0; i < 100; ++i) {
            for (size_t index = 0; index < workers_.size(); ++index) {
                Worker& worker = GetNextWorkerForTask(index);
                if (worker.GetState()->try_push(t)) {
                    OffsetIncrement();
                    return;
                }
            }
        }

        workers_.front().GetState()->push(t);
        OffsetIncrement();
    }

    void OffsetIncrement() {
        offset_.store((offset_.load() + 1) % workers_.size());
    }

    Worker& GetNextWorkerForTask(size_t index) {
        size_t worker_id = (index + offset_.load()) % workers_.size();
        return workers_.at(worker_id);
    }

private:
    std::atomic<std::size_t> offset_{0};
    std::vector<Worker> workers_;
};