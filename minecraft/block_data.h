#pragma once

#define TEXTURE_ATLAS_WIDTH 16
#define TEXTURE_ATLAS_HEIGHT 16

#define BLOCK_AIR 0
#define BLOCK_STONE 1
#define BLOCK_DIRT 2
#define BLOCK_GRASS 3

class BlockData {
public:
	// Texture coordinates
	float uSide, vSide;
	float uTop, vTop;
	float uBot, vBot;

	BlockData();
	BlockData(int xTex, int yTex);
	BlockData(int xSide, int ySide, int xTop, int yTop, int xBot, int yBot);
};

extern BlockData blockData[256];

void initBlockData();
