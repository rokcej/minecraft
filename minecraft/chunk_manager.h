#pragma once

#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>
#include "chunk.h"

struct ivec3Hash {
    // http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
    std::size_t operator()(const glm::ivec3& vec) const {
        return (vec.x * 11483) ^ (vec.y * 15787) ^ (vec.z * 19697);
    }
};

using ChunkMap = std::unordered_map<glm::ivec3, Chunk*, ivec3Hash>;

class ChunkManager {
public:
    int renderDistance = 10;
    ChunkMap chunks;
    glm::ivec3 lastChunkPos;

    ChunkManager();

    void update(glm::vec3 blockPos);

    Chunk* getChunk(int x, int y, int z);
    Chunk* createChunk(int x, int y, int z);
    void deleteChunk(ChunkMap::iterator it);

    bool isInRenderDistance(glm::ivec3 target, glm::ivec3 pos, int padding);
};
