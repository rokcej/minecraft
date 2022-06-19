#pragma once

#include <string>

struct GLFWwindow;
class State;

class Window {
public:
	Window(const std::string& title);

	void SetState(State* state);

private:
	static void FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height);
	static void KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double x, double y);

public:
	GLFWwindow* glfw_window_ = nullptr; // TODO: Make this private

private:
	State* state_ = nullptr;

};
