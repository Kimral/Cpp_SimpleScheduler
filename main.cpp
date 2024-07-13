#include <vector>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <random>

#include "ShedulersPool.h"

using VoidScheduler = Scheduler<std::function<void()>>;

int main() {
	SchedulersPool pool;
	pool.RegisterNewSheduler(1, new VoidScheduler{8});
	pool.RegisterNewSheduler(2, new VoidScheduler{1});

	pool.Start();

	int counter = 300;
	using namespace std::chrono_literals;
	while(counter--) {
		std::function<void()> task = []() {
			std::mt19937_64 eng{ std::random_device{}() };
			std::uniform_int_distribution<> dist{ 200, 1000 };
			std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });
		};
		VoidScheduler* scheduler = dynamic_cast<VoidScheduler*>(pool[1]);
		scheduler->AddTask(task);
	}
	counter = 50;
	while (counter--) {
		std::function<void()> task = []() {
			std::mt19937_64 eng{ std::random_device{}() };
			std::uniform_int_distribution<> dist{ 200, 1000 };
			std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });
		};
		VoidScheduler* scheduler = dynamic_cast<VoidScheduler*>(pool[2]);
		scheduler->AddTask(task);
	}

	using namespace std::chrono_literals;
	std::cout << "OK" << std::endl;
	return 0;
}