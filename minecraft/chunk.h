#pragma once

#include <vector>
#include <stdint.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define CHUNK_SIZE 16
#define CHUNK_VOLUME (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

int neighborsIndices[];

class Chunk {
public:
	glm::ivec3 pos;
	uint8_t data[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 }; // [z][y][x]
	Chunk* neighbors[6] = { nullptr }; // -x, +x, -y, +y, -z, +z

	GLuint vao = 0, vbo, ebo;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	unsigned int n_indices = 0;

	Chunk(int x, int y, int z);
	~Chunk();

	void generateMesh(); // Generates mesh from data
	void loadMesh(); // Loads mesh onto GPU

private:
	void generateData();
};
