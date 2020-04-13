#include "chunk_manager.h"

#include <chrono>
#include <vector>
#include <algorithm>
#include "chunk.h"
#include "camera.h"
#include "block_data.h"
#include "defines.h"

// Check if target is within Manhattan distance of pos
inline bool isWithinDistance(const glm::ivec3& target, const glm::ivec3& pos, const int distance) {
	return (
		target.x >= pos.x - distance && target.x <= pos.x + distance &&
		target.y >= pos.y - distance && target.y <= pos.y + distance &&
		target.z >= pos.z - distance && target.z <= pos.z + distance
	);
}

ChunkManager::ChunkManager(TerrainGenerator* tg) :
	terrainGen{ tg },
	lastChunkPos(1 << 31) // Min integer value
{
	initBlockData();

	// Spawn chunk loader thread
	chunkLoaderIsRunning = true;
	chunkLoaderThread = std::thread(chunkLoaderFunc, this);
}

ChunkManager::~ChunkManager() {
	// Terminate chunk loader thread
	std::unique_lock<std::mutex> mutexLock(chunkLoaderMutex);
	chunkLoaderIsRunning = false;
	mutexLock.unlock();
	chunkLoaderCondition.notify_all();
	chunkLoaderThread.join();

	// Delete chunks
	for (auto it = chunks.begin(); it != chunks.end(); /* No increment */)
		it = deleteChunk(it);
}

void ChunkManager::update(Camera* camera) {
	// TODO:
	// 1) Limit the number of chunks created, loaded and deleted per frame
	// 2) Create and load chunks radially, so that chunks nearest to the player are loaded first
	// 

	glm::ivec3 chunkPos = blockToChunkPos(camera->getPos()); // Current position in chunk coordinates

	// Process existing chunks
	for (auto it = chunks.begin(); it != chunks.end(); /* No increment */) {
		Chunk* c = it->second;
		if (c->isLoaded) { // Make sure chunk isn't being processed by secondary thread
			if (isWithinDistance(c->pos, chunkPos, camera->renderDistance)) { // Chunk in render distance
				// Load mesh
				if (c->meshGenerated && !c->meshLoaded) {
					c->loadMesh();
				}
			} else if (!isWithinDistance(c->pos, chunkPos, camera->renderDistance + 2)) { // Chunk outside render distance + 2
				// Delete chunk if possible
				bool safeToDelete = true;
				for (int side = 0; side < 6; ++side) {
					if (c->neighbors[side] != nullptr && !c->neighbors[side]->isLoaded) {
						safeToDelete = false;
						break;
					}
				}
				if (safeToDelete) {
					it = deleteChunk(it);
					// Don't increment iterator
					continue;
				}
			}
		}
		// Increment iterator
		++it;
	}

	if (chunkPos != lastChunkPos || camera->renderDistance != lastRenderDistance) {
		// Generate new chunks
		int generateDistance = camera->renderDistance + 1;

		// Put newly created chunks in shared queue
		std::unique_lock<std::mutex> mutexLock(chunkLoaderMutex);

		lastChunkPos = chunkPos;
		lastRenderDistance = camera->renderDistance;

		for (int x = chunkPos.x - generateDistance; x <= chunkPos.x + generateDistance; ++x) {
			for (int y = chunkPos.y - generateDistance; y <= chunkPos.y + generateDistance; ++y) {
				for (int z = chunkPos.z - generateDistance; z <= chunkPos.z + generateDistance; ++z) {
					glm::ivec3 pos(x, y, z);
					Chunk* c = getChunk(pos);
					if (c == nullptr) {
						c = createChunk(pos);
						chunkLoaderQueue.push(c);
					} else {
						if (c->isLoaded && !c->meshGenerated && isWithinDistance(c->pos, chunkPos, camera->renderDistance)) {
							c->isLoaded = false;
							chunkLoaderQueue.push(c);
						}
					}
				}
			}
		}

		mutexLock.unlock();
		chunkLoaderCondition.notify_all();
	}
}

Chunk* ChunkManager::getChunk(const glm::ivec3& chunkPos) const {
	auto it = chunks.find(chunkPos);
	if (it != chunks.end())
		return it->second;
	else
		return nullptr;
}

int ChunkManager::getBlock(const glm::ivec3& blockPos) const {
	glm::ivec3 chunkPos = blockToChunkPos(blockPos);
	glm::ivec3 blockPosRel = blockPos - chunkPos * CHUNK_SIZE;
	Chunk* c = getChunk(chunkPos);
	if (c != nullptr && c->dataGenerated)
		return c->data[blockPosRel.z][blockPosRel.y][blockPosRel.x];
	else
		return BlockType::AIR;
}

void ChunkManager::setBlock(const glm::ivec3& blockPos, int blockType) {
	glm::ivec3 chunkPos = blockToChunkPos(blockPos);
	glm::ivec3 blockPosRel = blockPos - chunkPos * CHUNK_SIZE;
	Chunk* c = getChunk(chunkPos);
	if (c != nullptr && c->isLoaded) {
		if (c->data[blockPosRel.z][blockPosRel.y][blockPosRel.x] != blockType) {
			// If block is on chunk border, get neighboring chunks
			std::vector<int> affectedSides;
			for (int i = 0; i < 3; ++i) {
				if (blockPosRel[i] == 0) affectedSides.push_back(2 * i);
				else if (blockPosRel[i] == CHUNK_SIZE - 1) affectedSides.push_back(2 * i + 1);
			}
			// Check if all neighbors are ready
			// TODO: Signal threads to reload the chunk instead of not doing anything
			bool safeToUpdate = true;
			for (int side : affectedSides) {
				if (!c->neighbors[side]->isLoaded) {
					safeToUpdate = false;
					break;
				}
			}
			// Update meshes if possible
			if (safeToUpdate) {
				c->data[blockPosRel.z][blockPosRel.y][blockPosRel.x] = blockType;
				for (int side : affectedSides) {
					c->neighbors[side]->generateMesh();
					c->neighbors[side]->loadMesh();
				}
				c->generateMesh();
				c->loadMesh();
			}
		}
	}
}

Chunk* ChunkManager::createChunk(const glm::ivec3& chunkPos) {
	Chunk* c = new Chunk(chunkPos);

	for (int side = 0; side < 6; ++side) {
		glm::ivec3 chunkPos2 = chunkPos + glm::ivec3(
			neighborsIndices[3 * side],
			neighborsIndices[3 * side + 1],
			neighborsIndices[3 * side + 2]
		);

		Chunk* c2 = getChunk(chunkPos2);
		if (c2 != nullptr) {
			int side2 = (side % 2 == 0) ? side + 1 : side - 1;
			c->neighbors[side] = c2;
			c2->neighbors[side2] = c;
		}
	}

	chunks.insert(std::make_pair(chunkPos, c));
	//chunks[glm::ivec3(x, y, z)] = c;
	return c;
}

ChunkMap::iterator ChunkManager::deleteChunk(ChunkMap::iterator it) {
	Chunk* c = it->second;
	delete c;
	return chunks.erase(it);
}

void chunkLoaderFunc(ChunkManager* cm) {
	std::queue<Chunk*> dataQueue;

	while (true) {
		// Read shared queue
		std::unique_lock<std::mutex> mutexLock(cm->chunkLoaderMutex);

		// Wait to be notified
		if (cm->chunkLoaderQueue.empty() && cm->chunkLoaderIsRunning)
			cm->chunkLoaderCondition.wait(mutexLock, [&cm]() { return !cm->chunkLoaderQueue.empty() || !cm->chunkLoaderIsRunning; });

		if (!cm->chunkLoaderIsRunning)
			break;

		glm::ivec3 chunkPos = cm->lastChunkPos;
		TerrainGenerator* terrainGen = cm->terrainGen;

		// Priority queue that loads meshes in order of increasing distance from player
		auto cmp = [&chunkPos](Chunk* &left, Chunk* &right) {\
			glm::vec3 l = left->pos - chunkPos;
			glm::vec3 r = right->pos - chunkPos;
			//return (l.x * l.x + l.y * l.y + l.z * l.z) > (r.x * r.x + r.y * r.y + r.z * r.z); // Euclidean distance
			return (abs(l.x) + abs(l.y) + abs(l.z)) > (abs(r.x) + abs(r.y) + abs(r.z)); // Manhattan distance
		};
		std::priority_queue<Chunk*, std::vector<Chunk*>, decltype(cmp)> meshQueue(cmp);

		// Get new chunks
		do {
			Chunk* c = cm->chunkLoaderQueue.front();
			cm->chunkLoaderQueue.pop();
			dataQueue.push(c);
		} while (!cm->chunkLoaderQueue.empty());

		mutexLock.unlock();

		// Generate data for new chunks
		while (!dataQueue.empty()) {
			Chunk* c = dataQueue.front();
			dataQueue.pop();

			if (!c->dataGenerated)
				c->generateData(terrainGen);

			if (isWithinDistance(c->pos, chunkPos, cm->lastRenderDistance))
				meshQueue.push(c);
			else
				c->isLoaded = true;
		}

		// Generate mesh for new chunks
		while (!meshQueue.empty()) {
			Chunk* c = meshQueue.top();
			meshQueue.pop();

			c->generateMesh();
			c->isLoaded = true;
		}

		// Clear chunk generation cache
		terrainGen->clearCache();
	}
}
