#include "chunk_manager.h"

#include <src/world/chunk.h>

ChunkManager::ChunkManager() {
	center_ = glm::ivec3(1 << 30); // TODO: Remove
}

ChunkManager::~ChunkManager() {

}

void ChunkManager::Update(glm::vec3 pos) {
	glm::ivec3 new_center(glm::floor(pos / (float)Chunk::kSize));
	if (center_ == new_center) {
		return;
	}
	center_ = new_center;

	glm::ivec3 min_bound = center_ - load_distance_;
	glm::ivec3 max_bound = center_ + load_distance_;
	glm::ivec3 i;
	for (i.x = min_bound.x; i.x <= max_bound.x; ++i.x) {
		for (i.y = min_bound.y; i.y <= max_bound.y; ++i.y) {
			for (i.z = min_bound.z; i.z <= max_bound.z; ++i.z) {
				const auto& it = chunks_.find(i);
				if (it != chunks_.end()) {
					continue;
				}
				chunks_.emplace(i, new Chunk(i));
			}
		}
	}

	for (auto it = chunks_.begin(); it != chunks_.end();) {
		int unload_distance = load_distance_ + unload_offset_;
		bool should_delete = glm::any(glm::greaterThan(glm::abs(it->first - center_), glm::ivec3(unload_distance)));
		if (!should_delete) {
			++it;
			continue;
		}
		it = chunks_.erase(it);
	}
}

Chunk* ChunkManager::GetChunk(glm::ivec3 index) const {
	const auto& it = chunks_.find(index);
	if (it != chunks_.end()) {
		return it->second.get();
	}
	return nullptr;
}

const ChunkManager::ChunkMap& ChunkManager::GetChunks() const {
	return chunks_;
}
