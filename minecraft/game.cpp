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

Game::Game() {
	prog = compileProgram(vsSourceTex, fsSourceTex);

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
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Game::update(float dt) {
	
}

void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			this->close = true;
		break;
	}
}
void Game::cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	
}
