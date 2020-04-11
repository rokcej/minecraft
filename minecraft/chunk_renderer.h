#pragma once

#include <GL/glew.h>

class Camera;
class ChunkManager;

class ChunkRenderer {
public:
	GLuint progChunk = 0;
	GLuint texChunk = 0;

	ChunkRenderer();
	~ChunkRenderer();

	void render(const Camera& camera, const ChunkManager& chunkManager);
};
