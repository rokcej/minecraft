#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Context {
public:
	bool close = false;

	Context() {}
	virtual ~Context() {}
	virtual void render() = 0;
	virtual void update(float dt) = 0;
	virtual void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}
	virtual void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
};
