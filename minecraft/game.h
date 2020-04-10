#pragma once

#include <glm/glm.hpp>
#include "context.h"
#include "player.h"
#include "chunk.h"
#include "chunk_manager.h"
#include "chunk_renderer.h"
#include "camera.h"

class Game : public Context {
private:
	Player player;
	Camera camera;
	ChunkManager chunkManager;
	ChunkRenderer chunkRenderer;

	float mouseSensitivity = 0.001f;
public:
	Game(GLFWwindow* window);
	~Game();
	void render() override;
	void update(float dt) override;

	void framebufferSizeCallback(GLFWwindow* window, int width, int height) override;
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
	void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) override;
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;
};
