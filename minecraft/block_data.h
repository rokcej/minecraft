#pragma once

#include <glm/glm.hpp>

#define TEXTURE_ATLAS_WIDTH 16
#define TEXTURE_ATLAS_HEIGHT 16

enum BlockType {
	AIR,
	STONE,
	DIRT,
	GRASS,
	SAND,
	WATER
};

class BlockData {
public:
	// Block parameters
	bool isTransparent;
	// Block textures
	float textureIndices[6];

	BlockData(bool transparent = false);
	BlockData(int row, int col, bool transparent = false);
	BlockData(int rowSide, int colSide, int rowTop, int colTop, int rowBot, int colBot, bool transparent = false);
};

extern BlockData blockData[TEXTURE_ATLAS_WIDTH * TEXTURE_ATLAS_HEIGHT];

void initBlockData();
