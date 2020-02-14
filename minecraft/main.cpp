#include <iostream>
#include <stack>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "game.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void windowRefreshCallback(GLFWwindow* window) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context != NULL) {
		context->render();
		glfwSwapBuffers(window);
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context != NULL) context->keyCallback(window, key, scancode, action, mods);
}
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context != NULL) context->cursorPosCallback(window, xPos, yPos);
}

void pushContext(GLFWwindow* window, std::stack<Context*>& stack, Context* context) {
	stack.push(context);
	glfwSetWindowUserPointer(window, context);
}
void popContext(GLFWwindow* window, std::stack<Context*>& stack) {
	delete stack.top();
	stack.pop();
	if (stack.empty())
		glfwSetWindowUserPointer(window, NULL);
	else
		glfwSetWindowUserPointer(window, stack.top());
}

int main() {
	// Init GLFW
	if (!glfwInit())
		return -1;
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Minecraft", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Init GLEW
	if (glewInit() != GLEW_OK) {
		return -1;
	}

	// GLFW Settings
	glfwSwapInterval(0); // Disable VSync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0.0, 0.0); // Center cursor for camera movement

	// GLFW Callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	// OpenGL flags
	//glEnable(GL_DEPTH_TEST);

	// Create context
	std::stack<Context*> contextStack;
	pushContext(window, contextStack, new Game());

	// Main loop
	while (!glfwWindowShouldClose(window) && !contextStack.empty()) {
		contextStack.top()->update(0.0f);
		contextStack.top()->render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (contextStack.top()->close)
			popContext(window, contextStack);
	}
	
	// Cleanup
	glfwTerminate();
	while (!contextStack.empty())
		popContext(window, contextStack);

	return 0;
}
