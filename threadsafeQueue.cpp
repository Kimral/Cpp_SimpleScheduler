#include "threadsafeQueue.h"

ThreadsafeQueue::ThreadsafeQueue(ThreadsafeQueue&& other) noexcept
{
	queue_.swap(other.queue_);
}

ThreadsafeQueue& ThreadsafeQueue::operator=(ThreadsafeQueue&& other) noexcept
{
	queue_.swap(other.queue_);
	return *this;
}

bool ThreadsafeQueue::empty()
{
	std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty();
}

bool ThreadsafeQueue::try_push(Task& t)
{
	std::unique_lock lock{ mutex_, std::try_to_lock };
	if (!lock)
		return false;

	queue_.emplace(std::move(t));
	return true;
}

void ThreadsafeQueue::push(Task& t)
{
	std::lock_guard lock{ mutex_ };
	queue_.emplace(std::move(t));
}

bool ThreadsafeQueue::try_pop(Task& t)
{
	std::lock_guard lock{ mutex_ };
	if (queue_.empty())
		return false;

	t = std::move(queue_.front());
	queue_.pop();
	return true;
}
