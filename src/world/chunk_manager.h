#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <src/utils/hash.h>

class Chunk;

// TODO: Think of a better name
class ChunkManager {

	using ChunkMap = std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, hash::Hash<glm::ivec3>>;

public:
	ChunkManager();
	~ChunkManager();

	void Update(glm::vec3 pos);

	Chunk* GetChunk(glm::ivec3 index) const;
	const ChunkMap& GetChunks() const;

private:
	ChunkMap chunks_;
	glm::ivec3 center_;

	int load_distance_ = 4;
	int unload_offset_ = 2;
};
