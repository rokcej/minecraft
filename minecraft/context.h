#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Context {
public:
	bool close = false;
	GLFWwindow* window;

	Context(GLFWwindow* window) : window{ window } {}
	virtual ~Context() {}

	virtual void render() = 0;
	virtual void update(float dt) = 0;

	virtual void framebufferSizeCallback(GLFWwindow* window, int width, int height) {}
	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}
	virtual void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
	virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}

	float getAspectRatio() {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return (float)w / (float)h;
	}

	int getWidth() {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return w;
	}

	int getHeight() {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return h;
	}
};
