#pragma once

#include "chunk_manager.h"

class ChunkRenderer {
public:
	GLuint prog = 0;
	GLuint tex = 0;

	ChunkRenderer();
	~ChunkRenderer();

	void render(const Camera& camera, const ChunkManager& chunkManager);
};
