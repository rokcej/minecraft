#pragma once

#include <string>

struct GLFWwindow;
class State;

class Window {
public:
	enum class CursorMode {
		kNormal = 0,
		kDisabled
	};

	Window(const std::string& title);

	void Close() const;
	int GetWidth() const;
	int GetHeight() const;

	void SetState(State* state);
	void SetCursorPos(double x, double y);
	void SetCursorMode(CursorMode mode);

private:
	static void FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height);
	static void KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double x, double y);

public:
	GLFWwindow* glfw_window_ = nullptr; // TODO: Make this private

private:
	int width_ = 1280;
	int height_ = 720;
	State* state_ = nullptr;

};
