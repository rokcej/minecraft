#include "block_data.h"

#include <iostream>
#include "defines.h"

bool blockDataInitialized = false;

BlockData blockData[256];

BlockData::BlockData(bool transparent) : BlockData(TEXTURE_ATLAS_WIDTH - 1, TEXTURE_ATLAS_HEIGHT - 1, transparent) {}
BlockData::BlockData(int row, int col, bool transparent) : BlockData(row, col, row, col, row, col, transparent) {}
BlockData::BlockData(int rowSide, int colSide, int rowTop, int colTop, int rowBot, int colBot, bool transparent) : isTransparent{ transparent } {
	// Side
	textureIndices[Side::LEFT] = (float)rowSide * (float)TEXTURE_ATLAS_WIDTH + (float)colSide;
	textureIndices[Side::RIGHT] = textureIndices[Side::LEFT];
	textureIndices[Side::BACK] = textureIndices[Side::LEFT];
	textureIndices[Side::FRONT] = textureIndices[Side::LEFT];
	// Top
	textureIndices[Side::TOP] = (float)rowTop * (float)TEXTURE_ATLAS_WIDTH + (float)colTop;
	// Bottom
	textureIndices[Side::BOT] = (float)rowBot * (float)TEXTURE_ATLAS_WIDTH + (float)colBot;
}


void initBlockData() {
	if (blockDataInitialized) {
		std::cout << "Block data already initialized!" << std::endl;
		return;
	}
	blockDataInitialized = true;

	blockData[BlockType::AIR] = BlockData(true);
	blockData[BlockType::STONE] = BlockData(0, 1);
	blockData[BlockType::DIRT] = BlockData(0, 2);
	blockData[BlockType::GRASS] = BlockData(0, 3, 0, 0, 0, 2);
	blockData[BlockType::SAND] = BlockData(1, 2);
	blockData[BlockType::WATER] = BlockData(13, 15, true);
}
