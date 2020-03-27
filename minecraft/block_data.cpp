#include "block_data.h"

#include <iostream>

BlockData blockData[256];

BlockData::BlockData() : BlockData(TEXTURE_ATLAS_WIDTH - 1, TEXTURE_ATLAS_HEIGHT - 1) {}
BlockData::BlockData(int row, int col) : BlockData(row, col, row, col, row, col) {}
BlockData::BlockData(int rowSide, int colSide, int rowTop, int colTop, int rowBot, int colBot) :
	sideTextureIndex { (float)rowSide * (float)TEXTURE_ATLAS_WIDTH + (float)colSide },
	topTextureIndex { (float)rowTop * (float)TEXTURE_ATLAS_WIDTH + (float)colTop },
	botTextureIndex { (float)rowBot * (float)TEXTURE_ATLAS_WIDTH + (float)colBot }

{}

bool blockDataInitialized = false;

void initBlockData() {
	if (blockDataInitialized) {
		std::cout << "Block data already initialized!" << std::endl;
		return;
	}
	blockDataInitialized = true;

	blockData[BLOCK_AIR] = BlockData();
	blockData[BLOCK_STONE] = BlockData(0, 0);
	blockData[BLOCK_DIRT] = BlockData(0, 1);
	blockData[BLOCK_GRASS] = BlockData(0, 3, 0, 2, 0, 1);
}
