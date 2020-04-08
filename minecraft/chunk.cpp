#include "chunk.h"

#include <math.h>
#include "block_data.h"
#include "defines.h"

const GLfloat blockVertices[] = { // Block vertices
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

const GLfloat faceTextureCoordinates[] = { // Block face texture coordinates
	0.f, 0.f, // Bottom left
	1.f, 0.f, // Bottom right
	1.f, 1.f, // Top right
	0.f, 1.f  // Top left
};

const GLuint faceIndices[] = {
	0, 1, 2,	0, 2, 3, // Block face indices
};

const int neighborsIndices[] = {
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
}

Chunk::~Chunk() {
	// Unlink neighbors
	for (int side = 0; side < 6; ++side) {
		if (neighbors[side] != nullptr) {
			int side2 = (side % 2 == 0) ? side + 1 : side - 1;
			neighbors[side]->neighbors[side2] = nullptr;
			neighbors[side] = nullptr;
		}
	}
	// Delete data
	vertices.clear();
	indices.clear();
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Chunk::generateData() {
	/*if (dataGenerated)
		std::cout << "ERROR: Data already generated" << std::endl;*/

	for (int x = 0; x < CHUNK_SIZE; ++x) {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
	for (int z = 0; z < CHUNK_SIZE; ++z) {
		int height = pos.y * CHUNK_SIZE + y + pos.x;
		int maxHeight = 52;
		if (pos.x == 0) {
			data[z][y][x] = BlockType::AIR;
		} else if (height == maxHeight) {
			data[z][y][x] = BlockType::GRASS;
		} else if (height < maxHeight - 32) {
			data[z][y][x] = BlockType::STONE;
		} else if (height < maxHeight) {
			data[z][y][x] = BlockType::DIRT;
		} else {
			data[z][y][x] = BlockType::AIR;
		}
	}
	}
	}

	dataGenerated = true;
}

void Chunk::generateMesh() {
	/*if (!dataGenerated)
		std::cout << "ERROR: Data not generated yet" << std::endl;
	if (meshGenerated)
		std::cout << "ERROR: Mesh already generated" << std::endl;*/

	vertices.clear();
	indices.clear();

	// Generate vertices and indices
	int index_offset = 0;
	for (int x = 0; x < CHUNK_SIZE; ++x) {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
	for (int z = 0; z < CHUNK_SIZE; ++z) {
		uint8_t blockType = data[z][y][x];

		if (blockType != BlockType::AIR) {
			GLfloat x_offset = (GLfloat)(x + pos.x * CHUNK_SIZE);
			GLfloat y_offset = (GLfloat)(y + pos.y * CHUNK_SIZE);
			GLfloat z_offset = (GLfloat)(z + pos.z * CHUNK_SIZE);

			for (int side = 0; side < 6; ++side) {
				int xn = x + neighborsIndices[3 * side + 0];
				int yn = y + neighborsIndices[3 * side + 1];
				int zn = z + neighborsIndices[3 * side + 2];

				uint8_t neighborType = BlockType::AIR;
				if (xn < 0 || xn >= CHUNK_SIZE || yn < 0 || yn >= CHUNK_SIZE || zn < 0 || zn >= CHUNK_SIZE) {
					if (neighbors[side] != nullptr && neighbors[side]->dataGenerated) {
						int xn2 = xn - neighborsIndices[3 * side + 0] * CHUNK_SIZE;
						int yn2 = yn - neighborsIndices[3 * side + 1] * CHUNK_SIZE;
						int zn2 = zn - neighborsIndices[3 * side + 2] * CHUNK_SIZE;
						neighborType = neighbors[side]->data[zn2][yn2][xn2];
					}/* else {
						std::cout << "ERROR: Can't create mesh because neighbor data is missing" << std::endl;
					}*/
				} else {
					neighborType = data[zn][yn][xn];
				}

				if (neighborType == BlockType::AIR) {
					// Texture coordinates
					float textureIndex = blockData[blockType].textureIndices[side];

					// Add vertices
					for (int j = 0; j < 4; ++j) {
						vertices.push_back(blockVertices[12 * side + 3 * j + 0] + x_offset);
						vertices.push_back(blockVertices[12 * side + 3 * j + 1] + y_offset);
						vertices.push_back(blockVertices[12 * side + 3 * j + 2] + z_offset);
						vertices.push_back(faceTextureCoordinates[2 * j + 0]);
						vertices.push_back(faceTextureCoordinates[2 * j + 1]);
						vertices.push_back(textureIndex);
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

	meshGenerated = true;
}

void Chunk::loadMesh() {
	/*if (!meshGenerated)
		std::cout << "ERROR: Mesh not generated yet" << std::endl;
	if (meshLoaded)
		std::cout << "ERROR: Mesh already loaded" << std::endl;*/

	// Put vertices and indices in vbo and ebo
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	n_indices = (unsigned int)indices.size();

	meshLoaded = true;
}

bool Chunk::hasAllNeighbors() {
	for (int side = 0; side < 6; ++side) {
		if (neighbors[side] == nullptr || !neighbors[side]->dataGenerated) {
			return false;
		}
	}
	return true;
}

glm::ivec3 blockToChunkPos(const glm::vec3& blockPos) {
	return glm::ivec3(
		(int)floorf(blockPos.x / (float)CHUNK_SIZE),
		(int)floorf(blockPos.y / (float)CHUNK_SIZE),
		(int)floorf(blockPos.z / (float)CHUNK_SIZE)
	);
}
