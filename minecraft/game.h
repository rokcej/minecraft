#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "context.h"
#include "shaders.h"
#include "png_reader.h"
#include "player.h"

class Game : public Context {
private:
	GLuint prog;
	GLuint vao, vbo, ebo, tex;
	glm::mat4 projMat, viewMat;
	Player player;
public:
	Game(GLFWwindow* window);
	~Game();
	void render() override;
	void update(float dt) override;

	void framebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) override;

	void updateViewMat();
	void updateProjMat();
};
