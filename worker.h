#pragma once

#include <vector>
#include <memory>

#include "common.h"
#include "ThreadState.h"

class ThreadState;

class Worker {
public:
    Worker(const std::vector<Worker>& pool, size_t relativePosition);
    ~Worker();

	Worker(const Worker& other);;
    void finish();
    void Start();
    std::unique_ptr<ThreadState>& GetState();
    bool TryStealTask(Task& t) const;

private:
    bool isAlive = true;
    std::unique_ptr<ThreadState> state_ = nullptr;
};