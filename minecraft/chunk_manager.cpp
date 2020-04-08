#include "chunk_manager.h"

#include <chrono>
#include <vector>
#include <algorithm>
#include "block_data.h"

// Check if target is within Manhattan distance of pos
inline bool isWithinDistance(const glm::ivec3& target, const glm::ivec3& pos, const int distance) {
	return (
		target.x >= pos.x - distance && target.x <= pos.x + distance &&
		target.y >= pos.y - distance && target.y <= pos.y + distance &&
		target.z >= pos.z - distance && target.z <= pos.z + distance
	);
}

ChunkManager::ChunkManager() :
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
					Chunk* c = getChunk(x, y, z);
					if (c == nullptr) {
						c = createChunk(x, y, z);
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

Chunk* ChunkManager::getChunk(int x, int y, int z) const {
	auto it = chunks.find(glm::ivec3(x, y, z));
	if (it != chunks.end())
		return it->second;
	else
		return nullptr;
}

Chunk* ChunkManager::createChunk(int x, int y, int z) {
	Chunk* c = new Chunk(x, y, z);

	for (int side = 0; side < 6; ++side) {
		int x2 = x + neighborsIndices[3 * side];
		int y2 = y + neighborsIndices[3 * side + 1];
		int z2 = z + neighborsIndices[3 * side + 2];

		Chunk* c2 = getChunk(x2, y2, z2);
		if (c2 != nullptr) {
			int side2 = (side % 2 == 0) ? side + 1 : side - 1;
			c->neighbors[side] = c2;
			c2->neighbors[side2] = c;
		}
	}

	chunks.insert(std::make_pair(glm::ivec3(x, y, z), c));
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
				c->generateData();

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
	}
}
