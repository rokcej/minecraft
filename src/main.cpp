#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <src/window.h>
#include <src/states/game_state.h>
#include <src/utils/timer.h>

int main() {
	// GLFW
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	Window window("Minecraft");
	glfwMakeContextCurrent(window.glfw_window_);
	glfwSwapInterval(0); // Disable VSync

	// GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Must be called after glfwMakeContextCurrent
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glEnable(GL_MULTISAMPLE);

	std::unique_ptr<State> state = std::make_unique<GameState>(&window);
	window.SetState(state.get());

	Timer timer;

	while (!glfwWindowShouldClose(window.glfw_window_)) {
		timer.Update();

		state->Update(timer.GetDeltaTime());
		state->Render();

		glfwSwapBuffers(window.glfw_window_);
		glfwPollEvents();
	}

	state = nullptr;
	glfwTerminate();

	return 0;
}
