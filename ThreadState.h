#pragma once

#include "threadsafeQueue.h"
class Worker;

class ThreadState {
public:
	ThreadState(const std::vector<Worker>& pool, size_t relativePosition);
    ~ThreadState();

	void AwaitNonEmptyState();
	bool TryToSteal(Task& t);
	void operator()();
	void finish();
	bool try_push(Task& t);
	bool try_pop(Task& t);
	void push(Task& t);

	std::thread& GetThread();

private:
    const std::vector<Worker>& pool_;
	size_t relativePosition_{static_cast<size_t>(-1)};

    std::atomic<bool> finish_{ false };
    std::mutex mutex_;
	std::condition_variable condition_;
	ThreadsafeQueue queue_;
	std::thread thread_;
};
