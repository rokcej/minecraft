#pragma once

#include <glm/glm.hpp>
#include "context.h"
#include "player.h"
#include "chunk_manager.h"
#include "chunk_renderer.h"
#include "outline_renderer.h"
#include "player_renderer.h"
#include "water_renderer.h"
#include "hud_renderer.h"
#include "camera.h"
#include "selection.h"
#include "terrain_generator.h"

class Game : public Context {
private:
	Player player;
	Camera camera;
	Selection selection;
	ChunkManager chunkManager;
	ChunkRenderer chunkRenderer;
	OutlineRenderer outlineRenderer;
	PlayerRenderer playerRenderer;
	WaterRenderer waterRenderer;
	HUDRenderer hudRenderer;
	TerrainGenerator terrainGen;

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
