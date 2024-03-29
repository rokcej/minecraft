#include "window.h"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/states/state.h>

Window::Window(const std::string& title) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	glfw_window_ = glfwCreateWindow(width_, height_, title.c_str(), NULL, NULL);
	if (!glfw_window_) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwSetWindowUserPointer(glfw_window_, this);
	glfwSetFramebufferSizeCallback(glfw_window_, FramebufferSizeCallback);
	glfwSetKeyCallback(glfw_window_, KeyCallback);
	glfwSetCursorPosCallback(glfw_window_, CursorPosCallback);
}

void Window::Close() const {
	glfwSetWindowShouldClose(glfw_window_, GLFW_TRUE);
}

int Window::GetWidth() const {
	return width_;
}

int Window::GetHeight() const {
	return height_;
}

void Window::SetState(State* state) {
	state_ = state;
}

void Window::SetCursorPos(double x, double y) {
	glfwSetCursorPos(glfw_window_, x, y);
}

void Window::SetCursorMode(CursorMode mode) {
	int glfw_mode;
	switch (mode) {
	case CursorMode::kNormal:
		glfw_mode = GLFW_CURSOR_NORMAL;
		break;
	case CursorMode::kDisabled:
		glfw_mode = GLFW_CURSOR_DISABLED;
		break;
	}
	glfwSetInputMode(glfw_window_, GLFW_CURSOR, glfw_mode);
}

void Window::FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
	if (window->state_) {
		window->state_->FramebufferSizeCallback(width, height);
	}

	window->width_ = width;
	window->height_ = height;
}

void Window::KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
	if (window->state_) {
		window->state_->KeyCallback(key, scancode, action, mods);
	}
}

void Window::CursorPosCallback(GLFWwindow* glfw_window, double x, double y) {
	Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
	if (window->state_) {
		window->state_->CursorPosCallback(x, y);
	}
}
