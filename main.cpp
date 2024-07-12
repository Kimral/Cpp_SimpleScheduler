#include <vector>
#include <thread>
#include <unordered_map>

#include "ShedulersPool.h"


int main() {
	ShedulersPool<std::function<void()>> pool;
	pool.RegisterNewSheduler(1, 4);

	pool.Start();

	int counter = 100;
	using namespace std::chrono_literals;
	while(counter--) {
		std::function<void()> task = []() {
			std::this_thread::sleep_for(100ms);
		};
		pool.PushTask(1, task);
	}

	using namespace std::chrono_literals;
	std::cout << "OK" << std::endl;
	return 0;
}