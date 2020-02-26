#include <iostream>
#include <stack>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "game.h"

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
	glfwSetWindowPos(window, (vidmode->width - windowWidth) * 0.5, (vidmode->height - windowHeight) * 0.5);
}

int main() {
	// Init GLFW
	if (!glfwInit())
		return -1;

	// Create window
	GLFWwindow* window = glfwCreateWindow(1600, 900, "Minecraft", NULL, NULL);
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

	// GLFW Settings
	glfwSwapInterval(0); // Disable VSync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0.0, 0.0); // Center cursor for camera movement

	// GLFW Callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	//glfwSetWindowFocusCallback(window, windowFocusCallback);

	// OpenGL flags
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create context
	std::stack<Context*> contextStack;
	pushContext(window, contextStack, new Game(window));

	// Timer
	auto timerStart = std::chrono::steady_clock::now();
	auto timerPrev = timerStart;
	auto timerFps = timerStart;
	int frameCount = 0;

	// Main loop
	while (!glfwWindowShouldClose(window) && !contextStack.empty()) {
		auto timerNow = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(timerNow - timerPrev).count(); // Time elapsed since last frame
		float t = std::chrono::duration<float>(timerNow - timerStart).count(); // Total time elapsed
		float fps_t = std::chrono::duration<float>(timerNow - timerFps).count();
		timerPrev = timerNow;
		if (fps_t >= 1.0f) {
			float fps = frameCount / fps_t;
			std::cout << "FPS: " << fps << std::endl;
			frameCount = 0;
			timerFps = timerNow;
		}
		++frameCount;

		contextStack.top()->update(dt);
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
