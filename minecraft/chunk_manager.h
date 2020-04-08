#pragma once

#include <unordered_map>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <glm/glm.hpp>
#include "chunk.h"
#include "camera.h"

// Chunk hash map data structure
struct ivec3Hash {
	// http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
	inline std::size_t operator()(const glm::ivec3& vec) const {
		return (vec.x * 11483) ^ (vec.y * 15787) ^ (vec.z * 19697);
	}
};
using ChunkMap = std::unordered_map<glm::ivec3, Chunk*, ivec3Hash>;

class ChunkManager {
public:
	ChunkMap chunks;

	// Last updated position and render distance
	glm::ivec3 lastChunkPos;
	int lastRenderDistance = -1;

	// Multithreading
	std::atomic<bool> chunkLoaderIsRunning = false;
	std::thread chunkLoaderThread;
	std::queue<Chunk*> chunkLoaderQueue;
	std::mutex chunkLoaderMutex;
	std::condition_variable chunkLoaderCondition;

	ChunkManager();
	~ChunkManager();

	void update(Camera* camera);

	Chunk* getChunk(int x, int y, int z) const;
	Chunk* createChunk(int x, int y, int z);
	ChunkMap::iterator deleteChunk(ChunkMap::iterator it);
};

void chunkLoaderFunc(ChunkManager* chunkManager);
