#pragma once

#include <vector>
#include <stdint.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define CHUNK_SIZE 16
#define CHUNK_VOLUME (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

class Chunk {
public:
	uint8_t data[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 }; // [z][y][x]

	GLuint vao, vbo, ebo;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	unsigned int n_indices = 0;

	Chunk(int x, int y, int z);
	~Chunk();

private:
	void generateData();
	void generateMesh();
};
