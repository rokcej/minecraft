#include "timer.h"

Timer::Timer() {
	Restart();
}

void Timer::Update() {
	time_prev_ = time_curr_;
	time_curr_ = std::chrono::steady_clock::now();

	t_ = std::chrono::duration<float>(time_curr_ - time_start_).count();
	dt_ = std::chrono::duration<float>(time_curr_ - time_prev_).count();
}

void Timer::Restart() {
	time_start_ = std::chrono::steady_clock::now();
	time_curr_ = time_start_;
	time_prev_ = time_start_;

	t_ = 0.0f;
	dt_ = 0.0f;
}

float Timer::GetTime() const {
	return t_;
}

float Timer::GetDeltaTime() const {
	return dt_;
}
