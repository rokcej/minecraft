#pragma once

#define TEXTURE_ATLAS_WIDTH 16
#define TEXTURE_ATLAS_HEIGHT 16

enum BlockType {
	AIR,
	STONE,
	DIRT,
	GRASS,
	SAND
};

class BlockData {
public:
	// Block textures
	float textureIndices[6];

	BlockData();
	BlockData(int row, int col);
	BlockData(int rowSide, int colSide, int rowTop, int colTop, int rowBot, int colBot);
};

extern BlockData blockData[TEXTURE_ATLAS_WIDTH * TEXTURE_ATLAS_HEIGHT];

void initBlockData();
