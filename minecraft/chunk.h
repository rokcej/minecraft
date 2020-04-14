#pragma once

#include <vector>
#include <atomic>
#include <stdint.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define CHUNK_SIZE 16
#define CHUNK_VOLUME (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

extern const int neighborsIndices[];

class TerrainGenerator;

class Chunk {
public:
	glm::ivec3 pos;
	std::atomic<uint8_t> data[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 }; // [z][y][x]
	Chunk* neighbors[6] = { nullptr }; // -x, +x, -y, +y, -z, +z

	GLuint vao[2] = { 0 }, vbo[2], ebo[2];
	std::vector<GLfloat> vertices[2];
	std::vector<GLuint> indices[2];
	unsigned int n_indices[2] = { 0 };

	std::atomic<bool> dataGenerated = false;
	std::atomic<bool> meshGenerated = false;
	std::atomic<bool> meshLoaded = false;

	std::atomic<bool> isLoaded = false;

	Chunk(const glm::ivec3& chunkPos);
	~Chunk();

	bool hasAllNeighbors();

	void generateData(TerrainGenerator* terrainGen); // Generates chunk data
	void generateMesh(); // Generates mesh from data
	void loadMesh(); // Loads mesh onto GPU
};

glm::ivec3 blockToChunkPos(const glm::vec3& blockPos);
bool isLegalBlockPos(const glm::ivec3& pos);
