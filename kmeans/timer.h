#pragma once

#include<chrono>

struct timer
{
	void restart();

	void stop();

	float seconds_last_stop();

	float seconds_so_far();

	std::chrono::steady_clock::time_point start_ { std::chrono::steady_clock::now() };
	std::chrono::steady_clock::time_point stop_;
};