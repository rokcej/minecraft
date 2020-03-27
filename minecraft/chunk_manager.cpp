#include "chunk_manager.h"

#include "block_data.h"

ChunkManager::ChunkManager() {
	initBlockData();

	for (int x = -renderDistance; x <= renderDistance; ++x) {
		for (int y = -renderDistance; y <= renderDistance; ++y) {
			for (int z = -renderDistance; z <= renderDistance; ++z) {
				//chunks.emplace(glm::ivec3(x, y, z), Chunk(x, y, z));
				createChunk(x, y, z);
			}
		}
	}

	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		it->second->generateMesh();
		it->second->loadMesh();
	}
}

Chunk* ChunkManager::getChunk(int x, int y, int z) {
	auto it = chunks.find(glm::ivec3(x, y, z));
	if (it != chunks.end())
		return it->second;
	else
		return nullptr;
}

void ChunkManager::createChunk(int x, int y, int z) {
	Chunk* c = new Chunk(x, y, z);

	for (int side = 0; side < 6; ++side) {
		int x2 = x + neighborsIndices[3 * side];
		int y2 = y + neighborsIndices[3 * side + 1];
		int z2 = z + neighborsIndices[3 * side + 2];

		auto it = chunks.find(glm::ivec3(x2, y2, z2));
		if (it != chunks.end()) {
			Chunk* c2 = it->second;
			int side2 = (side % 2 == 0) ? side + 1 : side - 1;
			c->neighbors[side] = c2;
			c2->neighbors[side2] = c;
		}
	}

	chunks.insert(std::make_pair(glm::ivec3(x, y, z), c));
}
