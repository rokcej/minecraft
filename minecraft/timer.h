#pragma once

#include <chrono>
#include <GLFW/glfw3.h>

class Timer {
public:
	Timer();

	void update();
	void restart();

	float time();
	float deltaTime();

private:
	using TimerClock = std::chrono::steady_clock;

	TimerClock::time_point startTime;
	TimerClock::time_point currentTime;
	TimerClock::time_point lastTime;
};

class FPSTimer {
public:
	float fps = 0.f;

	FPSTimer();

	void update(GLFWwindow* window);
	void render(GLFWwindow* window);

private:
	int frameCount = 0;
	Timer timer;
};
