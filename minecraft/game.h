#pragma once

#include <glm/glm.hpp>
#include "context.h"
#include "player.h"
#include "chunk.h"
#include "chunk_manager.h"

class Game : public Context {
private:
	GLuint prog;
	GLuint tex;
	glm::mat4 projMat, viewMat;
	Player player;
	ChunkManager chunkManager;
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
