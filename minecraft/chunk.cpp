#include "chunk.h"

#include <iostream>
#include "block_data.h"

#define SIDE_LEFT 0
#define SIDE_RIGHT 1
#define SIDE_BOT 2
#define SIDE_TOP 3
#define SIDE_BACK 4
#define SIDE_FRONT 5

GLfloat blockVertices[] = { // Block vertices
	// Left
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	// Right
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,
	// Bottom
	1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	// Top
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	// Back
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	// Front
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
};

GLfloat faceTextureCoordinates[] = { // Block face texture coordinates
	0.f, 0.f, // Bottom left
	1.f, 0.f, // Bottom right
	1.f, 1.f, // Top right
	0.f, 1.f  // Top left
};

GLuint faceIndices[] = {
	0, 1, 2,	0, 2, 3, // Block face indices
};

int neighborsIndices[] = {
	-1, 0, 0, +1, 0, 0, // Left, right (-x, +x)
	0, -1, 0, 0, +1, 0, // Bottom, top (-y, +y)
	0, 0, -1, 0, 0, +1  // Back, front (-z, +z)
};

Chunk::Chunk(int x, int y, int z) : pos{ x, y, z } {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	generateData();
}

Chunk::~Chunk() {
	for (int side = 0; side < 6; ++side) {
		if (neighbors[side] != nullptr) {
			int side2 = (side % 2 == 0) ? side + 1 : side - 1;
			neighbors[side]->neighbors[side2] = nullptr;
			neighbors[side] = nullptr;
		}
	}
}

void Chunk::generateData() {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
	for (int z = 0; z < CHUNK_SIZE; ++z) {
		int height = pos.y * CHUNK_SIZE + y;
		if (height == 48)
			data[z][y][x] = BLOCK_GRASS;
		else if (height < 32)
			data[z][y][x] = BLOCK_STONE;
		else if (height < 48)
			data[z][y][x] = BLOCK_DIRT;
		else
			data[z][y][x] = BLOCK_AIR;
	}
	}
	}
}

void Chunk::generateMesh() {
	vertices.clear();
	indices.clear();

	// Generate vertices and indices
	int index_offset = 0;
	for (int x = 0; x < CHUNK_SIZE; ++x) {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
	for (int z = 0; z < CHUNK_SIZE; ++z) {
		uint8_t blockType = data[z][y][x];

		if (blockType != BLOCK_AIR) {
			GLfloat x_offset = (GLfloat)(x + pos.x * CHUNK_SIZE);
			GLfloat y_offset = (GLfloat)(y + pos.y * CHUNK_SIZE);
			GLfloat z_offset = (GLfloat)(z + pos.z * CHUNK_SIZE);

			for (int side = 0; side < 6; ++side) {
				int xn = x + neighborsIndices[3 * side + 0];
				int yn = y + neighborsIndices[3 * side + 1];
				int zn = z + neighborsIndices[3 * side + 2];

				uint8_t neighborType = BLOCK_AIR;
				if (xn < 0 || xn >= CHUNK_SIZE || yn < 0 || yn >= CHUNK_SIZE || zn < 0 || zn >= CHUNK_SIZE) {
					if (neighbors[side] != nullptr) {
						int xn2 = xn - neighborsIndices[3 * side + 0] * CHUNK_SIZE;
						int yn2 = yn - neighborsIndices[3 * side + 1] * CHUNK_SIZE;
						int zn2 = zn - neighborsIndices[3 * side + 2] * CHUNK_SIZE;
						neighborType = neighbors[side]->data[zn2][yn2][xn2];
					}
				} else {
					neighborType = data[zn][yn][xn];
				}

				if (neighborType == BLOCK_AIR) {
					// Texture coordinates
					float texIndex;
					if (side == SIDE_TOP) {
						texIndex = blockData[blockType].topTextureIndex;
					} else if (side == SIDE_BOT) {
						texIndex = blockData[blockType].botTextureIndex;
					} else {
						texIndex = blockData[blockType].sideTextureIndex;
					}

					// Add vertices
					for (int j = 0; j < 4; ++j) {
						vertices.push_back(blockVertices[12 * side + 3 * j + 0] + x_offset);
						vertices.push_back(blockVertices[12 * side + 3 * j + 1] + y_offset);
						vertices.push_back(blockVertices[12 * side + 3 * j + 2] + z_offset);
						vertices.push_back(faceTextureCoordinates[2 * j + 0]);
						vertices.push_back(faceTextureCoordinates[2 * j + 1]);
						vertices.push_back(texIndex);
					}
					// Add indices
					for (int j = 0; j < 6; ++j) {
						indices.push_back(faceIndices[j] + index_offset);
					}
					index_offset += 4;
				}
			}
		}
	}
	}
	}
}

void Chunk::loadMesh() {
	// Put vertices and indices in vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	n_indices = (unsigned int)indices.size();
}
