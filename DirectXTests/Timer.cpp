#include "Timer.h"

Timer::Timer() {
	last = std::chrono::steady_clock::now();
}

long long Timer::Mark() {
	const auto old = last;
	last = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(last - old).count();
}

long long Timer::Peek() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last).count();
}