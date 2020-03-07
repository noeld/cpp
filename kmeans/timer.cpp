#include "timer.h"

void timer::restart()
{
	start_ = std::chrono::steady_clock::now() ;		
}

void timer::stop()
{
	stop_ = std::chrono::steady_clock::now();
}

float timer::seconds_last_stop()
{
	std::chrono::duration<float> d = stop_ - start_;
	return d.count();
}

float timer::seconds_so_far()
{
	std::chrono::duration<float> d = std::chrono::steady_clock::now() - start_;
	return d.count();
}
