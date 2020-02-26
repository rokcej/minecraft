#include "block_data.h"

#include <iostream>

BlockData blockData[256];

BlockData::BlockData() : BlockData(TEXTURE_ATLAS_WIDTH - 1, TEXTURE_ATLAS_HEIGHT - 1) {}
BlockData::BlockData(int xTex, int yTex) : BlockData(xTex, yTex, xTex, yTex, xTex, yTex) {}
BlockData::BlockData(int xSide, int ySide, int xTop, int yTop, int xBot, int yBot) : 
	uSide{ (float)xSide / TEXTURE_ATLAS_WIDTH },
	vSide{ (float)ySide / TEXTURE_ATLAS_HEIGHT },
	uTop{ (float)xTop / TEXTURE_ATLAS_WIDTH },
	vTop{ (float)yTop / TEXTURE_ATLAS_HEIGHT },
	uBot{ (float)xBot / TEXTURE_ATLAS_WIDTH },
	vBot{ (float)yBot / TEXTURE_ATLAS_HEIGHT }
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
	blockData[BLOCK_DIRT] = BlockData(1, 0);
	blockData[BLOCK_GRASS] = BlockData(3, 0, 2, 0, 1, 0);
}
