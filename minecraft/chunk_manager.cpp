#include "chunk_manager.h"

#include <chrono>
#include <vector>
#include <algorithm>
#include "chunk.h"
#include "camera.h"
#include "block_data.h"
#include "defines.h"
#include "collision.h"

// Check if target is within Manhattan distance of pos
bool isWithinSquare(const glm::ivec3& target, const glm::ivec3& pos, const int distance) {
	return (
		target.x >= pos.x - distance && target.x <= pos.x + distance &&
		target.y >= pos.y - distance && target.y <= pos.y + distance &&
		target.z >= pos.z - distance && target.z <= pos.z + distance
	);
}
// Check if target is within euclidean distance of pos
bool isWithinSphere(const glm::ivec3& target, const glm::ivec3& pos, const int distance) {
	glm::ivec3 rel = target - pos;
	int len2 = rel.x * rel.x + rel.y * rel.y + rel.z * rel.z;
	int dist2 = (distance + 1) * (distance + 1);
	return len2 < dist2 ;
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

void ChunkManager::update(const Camera& camera) {
	// TODO:
	// 1) Limit the number of chunks created, loaded and deleted per frame
	// 2) Create and load chunks radially, so that chunks nearest to the player are loaded first
	// 

	glm::ivec3 chunkPos = blockToChunkPos(camera.getPos()); // Current position in chunk coordinates
	std::queue<Chunk*> chunkQueue; // Chunks to be sent to the chunk loader queue

	// Reset list
	renderList.clear();

	// Max number of chunks updated per frame
	int maxGenerated = 128;
	int maxLoaded = 128;
	int maxDeleted = 128;

	// Generate new chunks
	if (!lastPosLoaded || chunkPos != lastChunkPos || camera.renderDistance != lastRenderDistance) {
		int distance = camera.renderDistance;

		glm::ivec3 diff = chunkPos - lastChunkPos;
		glm::ivec3 absDiff = glm::abs(diff);

		if (!lastPosLoaded || camera.renderDistance != lastRenderDistance || absDiff.x + absDiff.y + absDiff.z >= 2 * distance + 1) {
			// Loop through all chunks in range
			glm::ivec3 pos;
			for (pos.x = chunkPos.x - distance; pos.x <= chunkPos.x + distance; ++pos.x) {
				for (pos.y = chunkPos.y - distance; pos.y <= chunkPos.y + distance; ++pos.y) {
					for (pos.z = chunkPos.z - distance; pos.z <= chunkPos.z + distance; ++pos.z) {
						if (getChunk(pos) == nullptr) {
							if (maxGenerated > 0) {
								--maxGenerated;
								chunkQueue.push(createChunk(pos));
							} else {
								lastPosLoaded = false;
								goto break_loop;
							}
						}
					}
				}
			}
			lastChunkPos = chunkPos;
			lastPosLoaded = true;
		} else {
			// Loop only through chunks that weren't in range of last loaded position
			for (int i = 0; i < 3; ++i) {
				if (diff[i] != 0) {
					glm::ivec3 min = chunkPos - distance;
					glm::ivec3 max = chunkPos + distance;
					if (diff[i] > 0) min[i] = max[i] - diff[i] + 1;
					else if (diff[i] < 0) max[i] = min[i] - diff[i] - 1;

					glm::ivec3 pos;
					for (pos.x = min.x ; pos.x <= max.x; ++pos.x) {
						for (pos.y = min.y; pos.y <= max.y; ++pos.y) {
							for (pos.z = min.z; pos.z <= max.z; ++pos.z) {
								if (getChunk(pos) == nullptr) {
									if (maxGenerated > 0) {
										--maxGenerated;
										chunkQueue.push(createChunk(pos));
									} else {
										lastPosLoaded = false;
										goto break_loop;
									}
								}
							}
						}
					}
				}
			}
			lastChunkPos = chunkPos;
			lastPosLoaded = true;
		}
		// Break outer loop
	break_loop:
		lastRenderDistance = camera.renderDistance;
	}

	// Process existing chunks
	Frustum frustum(camera);
	for (auto it = chunks.begin(); it != chunks.end(); /* No increment */) {
		Chunk* c = it->second;
		if (c->isLoaded) { // Make sure chunk isn't being processed by secondary thread
			if (isWithinSphere(c->pos, chunkPos, camera.renderDistance)) { // Chunk in render distance
				// Generate mesh
				if (!c->meshGenerated) {
					if (maxGenerated > 0) {
						--maxGenerated;
						c->isLoaded = false;
						chunkQueue.push(c);
					}
				}
				// Load mesh
				else if (c->meshGenerated && !c->meshLoaded) {
					if (maxLoaded > 0) {
						--maxLoaded;
						c->loadMesh();
					}
				}
				// Add to render queue
				if (c->meshLoaded && (c->n_indices[0] > 0 || c->n_indices[1] > 0)) {
					AABB aabb(c);
					if (frustum.intersects(aabb)) {
						renderList.push_back(c);
					}
				}
			} else if (!isWithinSquare(c->pos, chunkPos, camera.renderDistance + 2)) { // Chunk outside render distance + 2
				if (maxDeleted > 0) {
					--maxDeleted;

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
		}
		// Increment iterator
		++it;
	}

	// Send chunks to the chunk loader queue
	if (!chunkQueue.empty()) {
		std::unique_lock<std::mutex> mutexLock(chunkLoaderMutex); // Lock mutex

		while (!chunkQueue.empty()) {
			// Move chunks from one queue to another
			Chunk* c = chunkQueue.front();
			chunkQueue.pop();
			chunkLoaderQueue.push(c);
			// If a chunk has a missing neighbor, create it and add it to the queue as well
			for (int side = 0; side < 6; ++side) {
				if (c->neighbors[side] == nullptr) {
					c->neighbors[side] = createChunk(getNeighborPos(side, c->pos));
					chunkLoaderQueue.push(c->neighbors[side]);
				}
			}
		}
		// Send position and render distance to chunk loader
		chunkLoaderPos = chunkPos;
		chunkLoaderDistance = camera.renderDistance;

		mutexLock.unlock(); // Unlock mutex
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

// Create new chunk, link existing neighbors and insert it into chunk map
Chunk* ChunkManager::createChunk(const glm::ivec3& chunkPos) {
	Chunk* c = new Chunk(chunkPos);
	// Link existing neighbors
	for (int side = 0; side < 6; ++side) {
		glm::ivec3 neighborPos = getNeighborPos(side, chunkPos);

		Chunk* cn = getChunk(neighborPos);
		if (cn != nullptr) {
			int neighborSide = (side % 2 == 0) ? side + 1 : side - 1;
			c->neighbors[side] = cn;
			cn->neighbors[neighborSide] = c;
		}
	}
	// Put chunk in map
	chunks.insert(std::make_pair(chunkPos, c));
	//chunks[glm::ivec3(x, y, z)] = c;

	return c;
}

ChunkMap::iterator ChunkManager::deleteChunk(const ChunkMap::iterator& it) {
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

		glm::ivec3 chunkPos = cm->chunkLoaderPos;
		int renderDistance = cm->chunkLoaderDistance;
		TerrainGenerator* terrainGen = cm->terrainGen;

		// Get new chunks
		do {
			/*Chunk* c = cm->chunkLoaderQueue.front();
			cm->chunkLoaderQueue.pop();
			dataQueue.push(c);*/
			dataQueue.swap(cm->chunkLoaderQueue);
		} while (!cm->chunkLoaderQueue.empty());

		mutexLock.unlock();

		// Priority queue that loads meshes in order of increasing distance from player
		auto cmp = [&chunkPos](Chunk*& left, Chunk*& right) {
			glm::vec3 l = left->pos - chunkPos;
			glm::vec3 r = right->pos - chunkPos;
			//return (l.x * l.x + l.y * l.y + l.z * l.z) > (r.x * r.x + r.y * r.y + r.z * r.z); // Euclidean distance
			return (abs(l.x) + abs(l.y) + abs(l.z)) > (abs(r.x) + abs(r.y) + abs(r.z)); // Manhattan distance
		};
		std::priority_queue<Chunk*, std::vector<Chunk*>, decltype(cmp)> meshQueue(cmp);

		// Generate data for new chunks
		while (!dataQueue.empty()) {
			Chunk* c = dataQueue.front();
			dataQueue.pop();

			if (!c->dataGenerated)
				c->generateData(terrainGen);

			if (c->hasAllNeighbors() && isWithinSphere(c->pos, chunkPos, renderDistance))
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
