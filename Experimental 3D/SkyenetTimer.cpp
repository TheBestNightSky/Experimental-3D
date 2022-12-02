#include "SkyenetTimer.h"

using namespace std::chrono;

SkyenetTimer::SkyenetTimer() {

	last = steady_clock::now();
}

float SkyenetTimer::Mark() {

	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();

}

float SkyenetTimer::Check() const {

	return duration<float>(steady_clock::now() - last).count();
}