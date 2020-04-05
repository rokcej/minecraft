#include "game.h"

#define _USE_MATH_DEFINES
#include <math.h>

constexpr float HALF_PI_LESS = ((float)M_PI_2 * 0.999f);

Game::Game(GLFWwindow* window) :
	Context(window),
	chunkManager(),
	chunkRenderer(),
	camera(&player)
{
	// Camera
	camera.updateProjMat(this->getAspectRatio());
}

Game::~Game() {

}

void Game::render() {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	chunkRenderer.render(&camera, &chunkManager);
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
	camera.updateProjMat((float)width / (float)height);
}
void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	// Debugging
	/// Filtering
	case GLFW_KEY_N:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		break;
	/// Mipmap level and anisotropic filtering level
	case GLFW_KEY_0:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 1.f);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
		}
		break;
	case GLFW_KEY_1:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 2.f);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 1);
		}
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 4.f);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 2);
		}
		break;
	case GLFW_KEY_3:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 8.f);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 3);
		}
		break;
	case GLFW_KEY_4:
		if (action == GLFW_PRESS) {
			if (mods & GLFW_MOD_SHIFT)
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 16.f);
			else
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 4);
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
