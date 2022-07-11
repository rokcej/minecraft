#pragma once

#include <cstdint>
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Chunk {
public:
	Chunk(glm::ivec3 index);
	~Chunk();

	void GenerateMesh();

private:
	int GetDataIndex(glm::ivec3 pos) const;

public:
	static inline constexpr int kSize = 16;
	static inline constexpr int kVolume = kSize * kSize * kSize;

	glm::ivec3 index_;
	std::array<uint8_t, kVolume> data_;

	GLuint vao_, vbo_, ebo_;
	unsigned int num_indices_ = 0;

};
