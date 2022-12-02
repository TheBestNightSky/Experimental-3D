#pragma once
#include <chrono>

class SkyenetTimer {

public:
	SkyenetTimer();
	float Mark();
	float Check() const;

private:
	std::chrono::steady_clock::time_point last;
};