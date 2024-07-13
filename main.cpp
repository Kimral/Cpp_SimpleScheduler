#include <vector>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <random>

#include "ShedulersPool.h"

using VoidScheduler = Scheduler<std::function<void()>>;

std::chrono::milliseconds GetMilliseconds(int min, int max) {
	std::mt19937_64 eng{ std::random_device{}() };
	std::uniform_int_distribution<> dist{ min, max };
	return std::chrono::milliseconds{dist(eng)};
}

int main() {
	SchedulersPool pool;
	pool.RegisterNewSheduler(1, new VoidScheduler{8});
	pool.RegisterNewSheduler(2, new VoidScheduler{4});
	pool.RegisterNewSheduler(3, new VoidScheduler{16});

	pool.Start();

	using namespace std::chrono_literals;
	int counter = 80;
	while(counter--) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [1] ";
		};
		VoidScheduler* scheduler = dynamic_cast<VoidScheduler*>(pool[1]);
		scheduler->AddTask(task);
	}

	counter = 30;
	while (counter--) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [2] ";
		};
		VoidScheduler* scheduler = dynamic_cast<VoidScheduler*>(pool[2]);
		scheduler->AddTask(task);
	}

	counter = 100;
	while (counter--) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(GetMilliseconds(200, 1000));
			std::cout << "Scheduler [3] ";
			};
		VoidScheduler* scheduler = dynamic_cast<VoidScheduler*>(pool[3]);
		scheduler->AddTask(task);
	}

	return 0;
}