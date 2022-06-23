#pragma once

#include <chrono>

class Timer {
public:
	Timer();

	void Update();
	void Restart();

	float GetTime() const;
	float GetDeltaTime() const;

private:
	float t_ = 0.0f;
	float dt_ = 0.0f;

	std::chrono::steady_clock::time_point time_start_;
	std::chrono::steady_clock::time_point time_curr_;
	std::chrono::steady_clock::time_point time_prev_;
};
