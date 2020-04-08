#include "timer.h"

// Timer
Timer::Timer() {
	startTime = TimerClock::now();
	currentTime = startTime;
	lastTime = startTime;
}

void Timer::update() {
	lastTime = currentTime;
	currentTime = TimerClock::now();
}

void Timer::restart() {
	startTime = currentTime;
	lastTime = currentTime;
}

float Timer::time() {
	return std::chrono::duration<float>(currentTime - startTime).count();
}

float Timer::deltaTime() {
	return std::chrono::duration<float>(currentTime - lastTime).count();
}

// FPS timer
FPSTimer::FPSTimer() : timer() {}

void FPSTimer::update(GLFWwindow* window) {
	timer.update();
	if (timer.time() > 1.f) {
		fps = (float)frameCount / timer.time();
		timer.restart();
		frameCount = 0;

		// TODO: Draw FPS on screen instead of changing window title
		render(window);
	}
	++frameCount;
}

void FPSTimer::render(GLFWwindow* window) {
	char title[64];
	snprintf(title, sizeof(title), "Minecraft (%.0f FPS)", fps);
	glfwSetWindowTitle(window, title);
}
