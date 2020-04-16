#pragma once

#include <unordered_map>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <glm/glm.hpp>
#include "hash.h"
#include "terrain_generator.h"

class Chunk;
class Camera;

using ChunkMap = std::unordered_map<glm::ivec3, Chunk*, ivec3Hash>;

class ChunkManager {
public:
	std::vector<Chunk*> renderList;

	ChunkMap chunks;
	TerrainGenerator* terrainGen;

	// Last updated position and render distance
	glm::ivec3 lastChunkPos;
	bool lastPosLoaded = true;
	int lastRenderDistance = 0;

	// Multithreading
	std::atomic<bool> chunkLoaderIsRunning = false;
	std::thread chunkLoaderThread;
	std::queue<Chunk*> chunkLoaderQueue;
	std::mutex chunkLoaderMutex;
	std::condition_variable chunkLoaderCondition;
	glm::ivec3 chunkLoaderPos;
	int chunkLoaderDistance = 0;

	ChunkManager(TerrainGenerator* tg);
	~ChunkManager();

	void update(const Camera& camera);

	Chunk* getChunk(const glm::ivec3& chunkPos) const;
	int getBlock(const glm::ivec3& blockPos) const;

	void setBlock(const glm::ivec3& blockPos, int blockType);

private:
	Chunk* createChunk(const glm::ivec3& chunkPos);
	ChunkMap::iterator deleteChunk(const ChunkMap::iterator& it);
};

void chunkLoaderFunc(ChunkManager* chunkManager);
