#include <vector>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <random>

#include "ShedulersPool.h"

int main() {
	SchedulersPool<std::function<void()>> pool;
	pool.RegisterNewSheduler(1, 8);

	pool.Start();

	int counter = 300;
	using namespace std::chrono_literals;
	while(counter--) {
		std::function<void()> task = []() {
			std::mt19937_64 eng{ std::random_device{}() };
			std::uniform_int_distribution<> dist{ 200, 1000 };
			std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });
		};
		pool.PushTask(1, task);
	}

	using namespace std::chrono_literals;
	std::cout << "OK" << std::endl;
	return 0;
}