#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/window.h>
#include <src/states/game_state.h>

int main() {
	// GLFW
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	Window window("Minecraft");
	glfwMakeContextCurrent(window.glfw_window_);

	// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD");
	}

	std::unique_ptr<State> state = std::make_unique<GameState>(&window);
	window.SetState(state.get());

	while (!glfwWindowShouldClose(window.glfw_window_)) {
		state->Update(0.0f);
		state->Render();

		glfwSwapBuffers(window.glfw_window_);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
