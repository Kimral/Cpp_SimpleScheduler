#include "worker.h"

#include "ThreadState.h"

Worker::Worker(const std::vector<Worker>& pool, size_t relativePosition) :
	state_{ std::make_unique<ThreadState>(pool, relativePosition) }
{}

Worker::~Worker()
{
	isAlive = false;
}

Worker::Worker(const Worker& other)
{
	isAlive = other.isAlive;
}

void Worker::finish()
{
	state_->finish();
}

void Worker::Start()
{
	state_->GetThread() = std::thread(std::ref(*state_));
}

std::unique_ptr<ThreadState>& Worker::GetState()
{
	return state_;
}

bool Worker::TryStealTask(Task& t) const
{
	return (isAlive) ? state_->try_pop(t) : false;
}
