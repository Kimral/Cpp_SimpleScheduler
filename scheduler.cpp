#include "scheduler.h"

#include "ThreadState.h"

#include <iostream>

Scheduler::Scheduler(size_t workers_count)
{
	workers_.reserve(workers_count);
	for(size_t index = 0; index < workers_count; ++index)
	{
		workers_.emplace_back(workers_, index);
	}
}

Scheduler::~Scheduler()
{
	for (auto& worker : workers_) {
		worker.finish();
	}
}

void Scheduler::Start()
{
	for (auto& worker : workers_) {
		worker.Start();
	}
}

void Scheduler::Push(Task t)
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

void Scheduler::OffsetIncrement()
{
	offset_.store((offset_.load() + 1) % workers_.size());
}

Worker& Scheduler::GetNextWorkerForTask(size_t index)
{
	size_t worker_id = (index + offset_.load()) % workers_.size();
	return workers_.at(worker_id);
}
