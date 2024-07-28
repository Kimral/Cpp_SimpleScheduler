#include "ThreadState.h"

#include "worker.h"

#include <iostream>

ThreadState::ThreadState(const std::vector<Worker>& pool, size_t relativePosition) :
	pool_{pool}, relativePosition_{ relativePosition }
{
}

ThreadState::~ThreadState()
{
	finish();
	if (thread_.joinable())
		thread_.join();
	std::cout << "tasks_done: " << tasks_done << std::endl;
}

void ThreadState::AwaitNonEmptyState()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]() {
		return finish_ || !queue_.empty();
	});
}

void ThreadState::operator()()
{
	Task task;
	for (;;) {
		if (queue_.try_pop(task)) {
			try {
				task();
				tasks_done++;
				continue;
			}
			catch (...) {
				std::cerr << "Exception caught during task execution.\n";
			}
		}
		else if (TryToSteal(task)) {
			task();
			tasks_done++;
			continue;
		}
		else if (finish_.load()) {
			break;
		}
		AwaitNonEmptyState();
	}
}

bool ThreadState::TryToSteal(Task& t)
{
	size_t counter = pool_.size();
	size_t diff = 0;
	while(--counter)
	{
		if (!queue_.empty())
			return false;
		const auto nextWorkerIndex = (++diff + relativePosition_) % pool_.size();
		if (pool_.at(nextWorkerIndex).TryStealTask(t))
		{
			std::cout << relativePosition_ << " stealled from " << nextWorkerIndex <<  std::endl;
			return true;
		}
	}
	return false;
}

void ThreadState::finish()
{
	finish_.store(true);
	condition_.notify_one();
}

bool ThreadState::try_push(Task& t)
{
	auto result = queue_.try_push(t);
	if (result) {
		condition_.notify_one();
	}
	return result;
}

bool ThreadState::try_pop(Task& t)
{
	return queue_.try_pop(t);
}

void ThreadState::push(Task& t)
{
	queue_.push(t);
	condition_.notify_one();
}

std::thread& ThreadState::GetThread()
{
	return thread_;
}
