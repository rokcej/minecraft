#include "window.h"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/states/state.h>

Window::Window(const std::string& title) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfw_window_ = glfwCreateWindow(1280, 720, title.c_str(), NULL, NULL);
	if (!glfw_window_) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwSetWindowUserPointer(glfw_window_, this);
	glfwSetFramebufferSizeCallback(glfw_window_, FramebufferSizeCallback);
	glfwSetKeyCallback(glfw_window_, KeyCallback);
	glfwSetCursorPosCallback(glfw_window_, CursorPosCallback);
}

void Window::SetState(State* state) {
	state_ = state;
}

void Window::FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {
	Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
	if (window->state_) {
		window->state_->FramebufferSizeCallback(width, height);
	}
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
