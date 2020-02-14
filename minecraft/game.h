#pragma once

#include "context.h"
#include "shaders.h"
#include "png_reader.h"

class Game : public Context {
private:
	GLuint prog;
	GLuint vao, vbo, ebo, tex;
public:
	Game();
	~Game();
	void render() override;
	void update(float dt) override;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) override;
};
