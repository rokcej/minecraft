#include "terrain_generator.h"

#include <math.h>
#include "block_data.h"

TerrainGenerator::TerrainGenerator() :
	maxHeight{ 200 },
	perlinNoise()
{
}

int TerrainGenerator::generate(int x, int y, int z) {
	TerrainData data = getTerrainData(x, z);
	int height = data.height;
	int offset = data.offset;

	int grassHeight = 55 + offset;
	int waterHeight = 35;

	if (y > height) {
		if (y > waterHeight)
			return BlockType::AIR;
		else
			return BlockType::WATER;
	} else if (y > grassHeight) {
		return BlockType::STONE;
	} else if (height < waterHeight + 2 && y > height - offset / 2) {
		return BlockType::SAND;
	} else if (y == height) {
		return BlockType::GRASS;
	} else {
		return BlockType::DIRT;
	}
}

TerrainData TerrainGenerator::getTerrainData(int x, int z) {
	glm::ivec2 key(x, z);
	auto it = terrainCache.find(key);
	if (it == terrainCache.end()) { // Terrain not cached yet
		// Generate terrain data
		TerrainData data;
		data.height = (int)(powf(perlinNoise.noise(x, z, 256, 6), 1.5f) * maxHeight);
		data.offset = (int)(perlinNoise.noise(x, z, 32, 3) * 48.f);
		// Cache generated value
		terrainCache.insert(std::make_pair(key, data));
		return data;
	} else { // Terrain already cached
		// Return cached value
		return it->second;
	}
}

// Clear the cache periodically to avoid storing too much unnecessary data
void TerrainGenerator::clearCache() {
	terrainCache.clear();
	perlinNoise.clearCache();
}
