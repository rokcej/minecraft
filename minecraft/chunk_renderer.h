#pragma once

#include "shaders.h"
#include "texture_loader.h"
#include "chunk_manager.h"

class ChunkRenderer {
public:
	GLuint prog = 0;
	GLuint tex = 0;

	ChunkRenderer();
	~ChunkRenderer();

	void render(Camera* camera, ChunkManager* chunkManager);
};
