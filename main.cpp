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

void CountTime(std::function<void()> func) {
	auto begin = std::chrono::steady_clock::now();
	func();
	auto end = std::chrono::steady_clock::now();

	double time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0;
	std::cout << "The time: " << std::fixed << std::setprecision(3) << time << " sec" << std::endl;
}

int main() {
	CountTime([]() {
		Scheduler first{ 400 };
		for (size_t index = 0; index < 30000; ++index) {
			first.push([index]() {
				std::this_thread::sleep_for(GetMilliseconds(100));
				std::cout << "TASK: " << index << std::endl;
			});
		}
		first.Start();
	});
	return 0;
}