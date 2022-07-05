#pragma once

#include <cstdint>
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Chunk {
public:
	Chunk(glm::vec3 pos);
	~Chunk();

	void GenerateMesh();

public:
	glm::vec3 pos_;

	std::array<uint8_t, 4096> data_;

	GLuint vao_, vbo_, ebo_;
	unsigned int num_indices_ = 0;

};
