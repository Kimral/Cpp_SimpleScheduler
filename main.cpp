#include <vector>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <random>
#include <iostream>

#include "scheduler.h"

std::chrono::milliseconds GetMilliseconds(int min, int max) {
	static std::mt19937_64 eng{ std::random_device{}() };
	static std::uniform_int_distribution<> dist{ min, max };
	return std::chrono::milliseconds{dist(eng)};
}

std::chrono::milliseconds GetMilliseconds(int time) {
	return std::chrono::milliseconds{ time };
}

int main() {

	Scheduler first{5};
	for (size_t index = 0; index < 100; ++index) {
		first.push([index]() {
			std::this_thread::sleep_for(GetMilliseconds(100));
			std::cout << "test: " << index << std::endl;
		});
	}
	first.Start();
	return 0;
}