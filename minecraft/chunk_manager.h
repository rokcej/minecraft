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


class ChunkManager {
public:
    int renderDistance = 4;

    std::unordered_map<glm::ivec3, Chunk*, ivec3Hash> chunks;

    ChunkManager();

    Chunk* getChunk(int x, int y, int z);
    void createChunk(int x, int y, int z);
};
