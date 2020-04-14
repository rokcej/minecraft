#pragma once
#include <GL/glew.h>

class Camera;
class ChunkManager;

class WaterRenderer {
public:
	GLuint progWater = 0;
	GLuint texWater = 0;

	WaterRenderer();
	~WaterRenderer();

	void render(const Camera& camera, const ChunkManager& chunkManager);
};
