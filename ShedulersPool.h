#pragma once

#include <unordered_map>
#include <iostream>

#include "Sheduler.h"

template <class TaskFunction>
class ShedulersPool {
public:
	ShedulersPool() = default;

	void RegisterNewSheduler(int sheduler_id, size_t threads_count) {
		if (!shedulers_.count(sheduler_id)) {
			shedulers_.emplace(sheduler_id, threads_count);
		}
		else {
			std::cout << threads_count << "already allocated" << std::endl;
		}
	}

	void PushTask(int sheduler_id, TaskFunction task) {
		shedulers_[sheduler_id].AddTask(task);
	}

	void Start() {
		for (auto& sheduler : shedulers_) {
			sheduler.second.Start();
		}
	}

private:
	std::unordered_map<int, Sheduler<TaskFunction>> shedulers_;
};