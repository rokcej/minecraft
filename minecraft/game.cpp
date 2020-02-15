#include "game.h"

GLfloat vertices[] = {
	-0.5f, +0.5f, 0.0f,	0.0f, 1.0f, // Top left
	-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, // Bottom left
	+0.5f, -0.5f, 0.0f,	1.0f, 0.0f, // Bottom right
	+0.5f, +0.5f, 0.0f,	1.0f, 1.0f  // Top right
};

GLuint indices[] = {
	0, 1, 2,
	0, 2, 3
};

Game::Game(GLFWwindow* window) : Context(window) {
	// Matrices
	updateViewMat();
	updateProjMat();

	// Shaders
	prog = compileProgram(vsSourceTex, fsSourceTex);

	// Objects
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	int w, h;
	GLubyte* image = read_png("data/stone.png", &w, &h, true);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	free_png(image);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Game::~Game() {
	
}

void Game::render() {
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUseProgram(prog);
	glBindVertexArray(vao);
	glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
	glm::mat4 pvmMat = projMat * viewMat;
	glUniformMatrix4fv(glGetUniformLocation(prog, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
