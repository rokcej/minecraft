#include <iostream>
#include <string.h>
#include <stack>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "game.h"
#include "timer.h"

#include "block_data.h"
#include "model.h"

#define WINDOW_TITLE "Minecraft"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define WINDOW_FULLSCREEN false
#define WINDOW_ANTI_ALIASING 8

void init() {
	initBlockData(); // Load block data
	initMeshData(); // Load mesh data
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context) context->framebufferSizeCallback(window, width, height);
}
void windowRefreshCallback(GLFWwindow* window) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context) {
		context->render();
		glfwSwapBuffers(window);
	}
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context) context->keyCallback(window, key, scancode, action, mods);
}
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context) context->cursorPosCallback(window, xPos, yPos);
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	Context* context = (Context*)glfwGetWindowUserPointer(window);
	if (context) context->mouseButtonCallback(window, button, action, mods);
}
void windowFocusCallback(GLFWwindow* window, int focused) {
	if (focused)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void pushContext(GLFWwindow* window, std::stack<Context*>& stack, Context* context) {
	stack.push(context);
	glfwSetWindowUserPointer(context->window, context);
}
void popContext(GLFWwindow* window, std::stack<Context*>& stack) {
	delete stack.top();
	stack.pop();
	if (stack.empty())
		glfwSetWindowUserPointer(window, NULL);
	else
		glfwSetWindowUserPointer(window, stack.top());
}

void centerWindow(GLFWwindow* window) {
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glfwSetWindowPos(window, (vidmode->width - windowWidth) / 2, (vidmode->height - windowHeight) / 2);
}

int main() {
	// Init GLFW
	if (!glfwInit())
		return -1;

	// Create window
	glfwWindowHint(GLFW_SAMPLES, WINDOW_ANTI_ALIASING);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WINDOW_FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	centerWindow(window);
	glfwMakeContextCurrent(window);

	// Init GLEW
	if (glewInit() != GLEW_OK) {
		return -1;
	}

	// My init
	init();

	// Print versions
	std::cout << "OpenGL version:\t" << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLEW version:\t" << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GLFW version:\t" << glfwGetVersionString() << std::endl;
	std::cout << std::endl;

	// GLFW settings
	glfwSwapInterval(0); // Disable VSync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0.0, 0.0); // Center cursor for camera movement

	// GLFW callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	//glfwSetWindowFocusCallback(window, windowFocusCallback);

	// OpenGL flags
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create context
	std::stack<Context*> contextStack;
	pushContext(window, contextStack, new Game(window));
	
	// Timers
	Timer timer;
	FPSTimer fpsTimer;

	// Main loop
	while (!glfwWindowShouldClose(window) && !contextStack.empty()) {
		timer.update();
		fpsTimer.update(window);

		Context* ctx = contextStack.top();
		ctx->update(timer.deltaTime());
		ctx->render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (ctx->close)
			popContext(window, contextStack);
	}

	// Cleanup
	glfwTerminate();
	while (!contextStack.empty())
		popContext(window, contextStack);

	return 0;
}
