#pragma once

#include <unordered_map>
#include <iostream>

#include "Sheduler.h"

class SchedulersPool {
public:
	SchedulersPool() = default;
	~SchedulersPool() {
		for (auto& sheduler : shedulers_) {
			delete sheduler.second;
		}
	}

	void RegisterNewSheduler(int sheduler_id, SchedulerInterface* scheduler) {
		if (!shedulers_.count(sheduler_id)) {
			shedulers_.emplace(sheduler_id, scheduler);
		}
		else {
			std::cout << sheduler_id << "already allocated" << std::endl;
		}
	}

	void Start() {
		for (auto& sheduler : shedulers_) {
			sheduler.second->Start();
		}
	}

	SchedulerInterface* operator[](int index) {
		return shedulers_.at(index);
	}

private:
	std::unordered_map<int, SchedulerInterface*> shedulers_;
};