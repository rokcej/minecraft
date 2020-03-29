#include "chunk_manager.h"

#include "block_data.h"

ChunkManager::ChunkManager() {
	initBlockData();

	//for (int x = -renderDistance; x <= renderDistance; ++x) {
	//	for (int y = -renderDistance; y <= renderDistance; ++y) {
	//		for (int z = -renderDistance; z <= renderDistance; ++z) {
	//			//chunks.emplace(glm::ivec3(x, y, z), Chunk(x, y, z));
	//			createChunk(x, y, z);
	//		}
	//	}
	//}

	//for (auto it = chunks.begin(); it != chunks.end(); ++it) {
	//	it->second->generateMesh();
	//	it->second->loadMesh();
	//}
}

void ChunkManager::update(glm::vec3 blockPos) {
	glm::ivec3 chunkPos = blockToChunkPos(blockPos);
	if (chunkPos != lastChunkPos) {
		// Generate
		for (int x = chunkPos.x - renderDistance - 1; x <= chunkPos.x + renderDistance + 1; ++x) {
			for (int y = chunkPos.y - renderDistance - 1; y <= chunkPos.y + renderDistance + 1; ++y) {
				for (int z = chunkPos.z - renderDistance - 1; z <= chunkPos.z + renderDistance + 1; ++z) {
					if (getChunk(x, y, z) == nullptr) {
						Chunk* c = createChunk(x, y, z);
						c->generateData();
					}
				}
			}
		}

		// Process
		for (auto it = chunks.begin(); it != chunks.end(); ++it) {
			Chunk* c = it->second;
			if (isInRenderDistance(c->pos, chunkPos, 0)) { // Load
				if (!c->meshGenerated)
					c->generateMesh();
				if (!c->meshLoaded)
					c->loadMesh();
			} else if (isInRenderDistance(c->pos, chunkPos, 1)) { // Update
				// Nothing yet
			} else { // Delete
				deleteChunk(it);
			}
		}

		lastChunkPos = chunkPos;
	}
}

Chunk* ChunkManager::getChunk(int x, int y, int z) {
	auto it = chunks.find(glm::ivec3(x, y, z));
	if (it != chunks.end())
		return it->second;
	else
		return nullptr;
}

Chunk* ChunkManager::createChunk(int x, int y, int z) {
	Chunk* c = new Chunk(x, y, z);
	c->generateData();

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
	return c;
}

void ChunkManager::deleteChunk(ChunkMap::iterator it) {
	delete it->second;
	chunks.erase(it);
}

bool ChunkManager::isInRenderDistance(glm::ivec3 target, glm::ivec3 pos, int padding) {
	int range = renderDistance + padding;
	return (
		target.x >= pos.x - range && target.x <= pos.x + range &&
		target.y >= pos.y - range && target.y <= pos.y + range &&
		target.z >= pos.z - range && target.z <= pos.z + range
	);
}
