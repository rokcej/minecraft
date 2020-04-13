#pragma once

#include "perlin_noise.h"

struct TerrainData {
	int height;
	int offset;
};

class TerrainGenerator {
public:
	TerrainGenerator();

	int generate(int x, int y, int z);
	TerrainData getTerrainData(int x, int z);

	void clearCache();

private:
	int maxHeight;
	PerlinNoise perlinNoise;

	std::unordered_map<glm::ivec2, TerrainData, ivec2Hash> terrainCache;
};
