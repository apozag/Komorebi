#pragma once

#include <chrono>

class Timer {
private:
	std::chrono::steady_clock::time_point last;
public:
	Timer();
	long long Mark();
	long long Peek();
};
