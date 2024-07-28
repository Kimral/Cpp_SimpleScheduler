#pragma once

#include "common.h"
#include "worker.h"

class Scheduler {
public:
    Scheduler() = default;
    explicit Scheduler(size_t workers_count);
    ~Scheduler();

    void Start();
    void Push(Task t);
    void OffsetIncrement();
    Worker& GetNextWorkerForTask(size_t index);

private:
    std::atomic<std::size_t> offset_{0};
    std::vector<Worker> workers_;
};