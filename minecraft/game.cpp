#include "game.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include "png_reader.h"
#include "block_data.h"

Game::Game(GLFWwindow* window) : Context(window), chunk(0, 0, 0) {
	// Block types
	initBlockData();

	// Matrices
	updateViewMat();
	updateProjMat();

	// Shaders
	prog = compileProgram(vsSourceTex, fsSourceTex);

	// Textures
	int w, h;
	GLubyte* image = read_png("data/textures.png", &w, &h, true);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	free_png(image);
}

Game::~Game() {
	
}

void Game::render() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUseProgram(prog);
	glBindVertexArray(chunk.vao);
	glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
	glm::mat4 pvmMat = projMat * viewMat;
	glUniformMatrix4fv(glGetUniformLocation(prog, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));
	glDrawElements(GL_TRIANGLES, chunk.n_indices, GL_UNSIGNED_INT, 0);
}

void Game::update(float dt) {
	// Position
	glm::vec3 right = glm::vec3(cosf(player.rot.y), 0.0f, -sinf(player.rot.y)); // glm::cross(player.forward, player.up);
	if (player.move.x != 0.0f) // Right
		player.pos += right * (player.move.x * player.speed * dt);
	if (player.move.y != 0.0f) // Up
		player.pos += player.up * (player.move.y * player.speed * dt);
	if (player.move.z != 0.0f) // Forward
		player.pos += player.forward * (player.move.z * player.speed * dt);

	// Rotation
	player.forward = glm::vec3(
		-sinf(player.rot.y) * cosf(player.rot.x),
		sinf(player.rot.x),
		-cosf(player.rot.y) * cosf(player.rot.x)
	);

	updateViewMat();
}

void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	updateProjMat();
}
void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
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
	}
}
void Game::cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	player.rot.y -= (float)xPos * player.mouseSensitivity; // Yaw
	player.rot.x -= (float)yPos * player.mouseSensitivity; // Pitch

	// Constrain yaw to [0, 2*PI]
	player.rot.y -= 2.0f * (float)M_PI * floorf(player.rot.y / (2.0f * (float)M_PI));

	// Limit pitch to [-PI/2, PI/2]
	if (player.rot.x > (float)M_PI_2)
		player.rot.x = (float)M_PI_2;
	if (player.rot.x < -(float)M_PI_2)
		player.rot.x = -(float)M_PI_2;

	glfwSetCursorPos(window, 0.0, 0.0);
}

void Game::updateViewMat() {
	viewMat = glm::lookAt(
		player.pos,
		player.pos + player.forward,
		player.up
	);
}
void Game::updateProjMat() {
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	projMat = glm::perspective(player.fov, (float)w / (float)h, 0.1f, 1000.0f);
}
