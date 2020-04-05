#pragma once

#define TEXTURE_ATLAS_WIDTH 16
#define TEXTURE_ATLAS_HEIGHT 16

#define BLOCK_AIR 0
#define BLOCK_STONE 1
#define BLOCK_DIRT 2
#define BLOCK_GRASS 3

class BlockData {
public:
	// Block textures
	float sideTextureIndex;
	float topTextureIndex;
	float botTextureIndex;

	BlockData();
	BlockData(int row, int col);
	BlockData(int rowSide, int colSide, int rowTop, int colTop, int rowBot, int colBot);
};

extern BlockData blockData[TEXTURE_ATLAS_WIDTH * TEXTURE_ATLAS_HEIGHT];

void initBlockData();
