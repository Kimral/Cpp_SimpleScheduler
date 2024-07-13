#include <vector>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <random>
#include <iostream>

#include "Sheduler.h"

std::chrono::milliseconds GetMilliseconds(int min, int max) {
	static std::mt19937_64 eng{ std::random_device{}() };
	static std::uniform_int_distribution<> dist{ min, max };
	return std::chrono::milliseconds{dist(eng)};
}

int main() {
	Scheduler first{ 8 };
	Scheduler second{ 8 };
	Scheduler third{ 8 };;

	first.Start();
	second.Start();
	third.Start();

	using namespace std::chrono_literals;

	for (size_t index = 0; index < 80; ++index) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [1] ";
			};
		first.AddTask(task);
	}

	for (size_t index = 0; index < 30; ++index) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [2] ";
		};
		second.AddTask(task);
	}

	for (size_t index = 0; index < 100; ++index) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [3] ";
		};
		third.AddTask(task);
	}

	return 0;
}