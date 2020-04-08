#include "game.h"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

constexpr float HALF_PI_LESS = ((float)M_PI_2 * 0.999f);

Game::Game(GLFWwindow* window) :
	Context(window),
	chunkManager(),
	chunkRenderer(),
	camera(&player, getAspectRatio())
{}

Game::~Game() {

}

void Game::render() {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	chunkRenderer.render(camera, chunkManager);
}

void Game::update(float dt) {
	// Player and camera
	player.update(dt);
	camera.updateViewMat();

	// Chunks
	chunkManager.update(&camera);
}

// Callbacks
void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	camera.setAspectRatio((float)width / (float)height);
}
void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	// Debugging
	/// Third person
	case GLFW_KEY_T:
		if (action == GLFW_PRESS) { // Toggle third person mode
			camera.thirdPerson = !camera.thirdPerson;
		}
		break;
	/// Chunks
	case GLFW_KEY_R:
		if (action == GLFW_PRESS) { // Reload chunks
			for (auto it = chunkManager.chunks.begin(); it != chunkManager.chunks.end(); )
				it = chunkManager.deleteChunk(it);
			chunkManager.lastChunkPos = glm::ivec3(1 < 31);
		}
		break;
	/// Fog
	case GLFW_KEY_F:
		if (action == GLFW_PRESS) { // Toggle fog
			camera.enableFog = !camera.enableFog;
		}
		break;
	case GLFW_KEY_MINUS:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT) { // Decrease fog depth
				--camera.fogDepth;
				if (camera.fogDepth < 0)
					camera.fogDepth = 0;
			} else { // Decrease render distance
				--camera.renderDistance;
				if (camera.renderDistance < 0)
					camera.renderDistance = 0;
				if (camera.fogDepth > camera.renderDistance)
					camera.fogDepth = camera.renderDistance;
				camera.updateProjMat();
			}
		}
		break;
	case GLFW_KEY_EQUAL:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT) { // Increase fog depth
				++camera.fogDepth;
				if (camera.fogDepth > camera.renderDistance)
					camera.fogDepth = camera.renderDistance;
			} else { // Increase render distance
				++camera.renderDistance;
				camera.updateProjMat();
			}
		}
		break;
	// UI
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			this->close = true;
		break;

	// Movement
	case GLFW_KEY_D: // Right
		if (action == GLFW_PRESS)
			player.move.x += 1;
		else if (action == GLFW_RELEASE)
			player.move.x -= 1;
		break;
	case GLFW_KEY_A: // Left
		if (action == GLFW_PRESS)
			player.move.x -= 1;
		else if (action == GLFW_RELEASE)
			player.move.x += 1;
		break;
	case GLFW_KEY_SPACE: // Up
		if (action == GLFW_PRESS)
			player.move.y += 1;
		else if (action == GLFW_RELEASE)
			player.move.y -= 1;
		break;
	case GLFW_KEY_LEFT_CONTROL: // Down
		if (action == GLFW_PRESS)
			player.move.y -= 1;
		else if (action == GLFW_RELEASE)
			player.move.y += 1;
		break;
	case GLFW_KEY_W: // Forward
		if (action == GLFW_PRESS)
			player.move.z += 1;
		else if (action == GLFW_RELEASE)
			player.move.z -= 1;
		break;
	case GLFW_KEY_S: // Backward
		if (action == GLFW_PRESS)
			player.move.z -= 1;
		else if (action == GLFW_RELEASE)
			player.move.z += 1;
		break;
	case GLFW_KEY_LEFT_SHIFT: // Sprint
		if (action == GLFW_PRESS)
			player.speed *= 3;
		else if (action == GLFW_RELEASE)
			player.speed /= 3;
		break;
	}
}
void Game::cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	player.rot.y -= (float)xPos * mouseSensitivity; // Yaw
	player.rot.x -= (float)yPos * mouseSensitivity; // Pitch

	// Constrain yaw to [0, 2*PI]
	player.rot.y -= 2.0f * (float)M_PI * floorf(player.rot.y / (2.0f * (float)M_PI));

	// Limit pitch to [-PI/2, PI/2]
	if (player.rot.x > HALF_PI_LESS)
		player.rot.x = HALF_PI_LESS;
	if (player.rot.x < -HALF_PI_LESS)
		player.rot.x = -HALF_PI_LESS;

	glfwSetCursorPos(window, 0.0, 0.0);
}
