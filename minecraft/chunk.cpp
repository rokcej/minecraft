#include "chunk.h"

#include <math.h>
#include "block_data.h"
#include "defines.h"
#include "terrain_generator.h"

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

const GLfloat blockNormals[] = {
	-1.f, 0.0f, 0.0f, // Left
	+1.f, 0.0f, 0.0f, // Right
	0.0f, -1.f, 0.0f, // Bottom
	0.0f, +1.f, 0.0f, // Top
	0.0f, 0.0f, -1.f, // Back
	0.0f, 0.0f, +1.f  // Front
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

const int neighborIndexMap[][6] = {
	// Left, right, bottom, top, back, front
	{ 0, 0, 1, 1, 2, 2 }, // Dimension index
	{ -1, +1, -1, +1, -1, +1 } // Dimension offset
};

Chunk::Chunk(const glm::ivec3& chunkPos) : pos(chunkPos) {
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);
	glGenBuffers(2, ebo);

	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);

		glEnableVertexAttribArray(0); // Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

		glEnableVertexAttribArray(1); // Texture coordinates
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(2); // Normals
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	}

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
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(2, ebo);
	glDeleteVertexArrays(2, vao);
}

void Chunk::generateData(TerrainGenerator* terrainGen) {
	/*if (dataGenerated)
		std::cout << "ERROR: Data already generated" << std::endl;*/

	glm::ivec3 blockPos = pos * CHUNK_SIZE;

	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				data[z][y][x] = terrainGen->generate(blockPos.x + x, blockPos.y + y, blockPos.z + z);
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

	for (int i = 0; i < 2; ++i) {
		vertices[i].clear();
		indices[i].clear();
	}

	// Generate vertices and indices
	int index_offset[2] = { 0 };
	glm::ivec3 ip;
	for (ip.x = 0; ip.x < CHUNK_SIZE; ++ip.x) {
	for (ip.y = 0; ip.y < CHUNK_SIZE; ++ip.y) {
	for (ip.z = 0; ip.z < CHUNK_SIZE; ++ip.z) {
		uint8_t blockType = data[ip.z][ip.y][ip.x]; // Get current block type

		if (blockType != BlockType::AIR) {
			glm::vec3 offset = (glm::vec3)(ip + pos * CHUNK_SIZE); // Get current block's absolute position

			for (int side = 0; side < 6; ++side) {
				// Find neighboring block's position and chunk (can be either the same chunk or neighboring chunk)
				// We assume the neighboring chunk always exists
				glm::ivec3 ipn = getNeighborPos(side, ip);
				Chunk* cn;

				int dimIndex = neighborIndexMap[0][side]; // Which dimension is the neighbor in
				int dimDirection = neighborIndexMap[1][side]; // Which direction in the dimension is the neighbor in
				if (ipn[dimIndex] < 0 || ipn[dimIndex] >= CHUNK_SIZE) {
					ipn[dimIndex] -= dimDirection * CHUNK_SIZE;
					cn = neighbors[side];
				} else {
					cn = this;
				}

				// Get neighboring block type
				uint8_t neighborType = cn->data[ipn.z][ipn.y][ipn.x];

				if (blockType == BlockType::WATER) { // Create water mesh
					// Water
					if (neighborType != BlockType::WATER && (blockData[neighborType].isTransparent || side == Side::TOP)) {
						// Texture coordinates
						float textureIndex = blockData[blockType].textureIndices[side];

						// Spaghetti code for water height
						uint8_t topNeighbor;
						if (ip.y + 1 < CHUNK_SIZE)
							topNeighbor = data[ip.z][ip.y + 1][ip.x];
						else
							topNeighbor = neighbors[Side::TOP]->data[ip.z][ip.y + 1 - CHUNK_SIZE][ip.x];
						float waterHeight = 1.f;
						if (topNeighbor != BlockType::WATER)
							waterHeight = .8f;

						// Add vertices
						for (int j = 0; j < 4; ++j) {
							// Position
							vertices[1].push_back(blockVertices[12 * side + 3 * j + 0] + offset.x);
							vertices[1].push_back(waterHeight * blockVertices[12 * side + 3 * j + 1] + offset.y);
							vertices[1].push_back(blockVertices[12 * side + 3 * j + 2] + offset.z);
							// Texture coords
							vertices[1].push_back(faceTextureCoordinates[2 * j + 0]);
							if (side == Side::TOP || side == Side::BOT)
								vertices[1].push_back(faceTextureCoordinates[2 * j + 1]);
							else
								vertices[1].push_back(waterHeight * faceTextureCoordinates[2 * j + 1]);
							vertices[1].push_back(textureIndex);
							// Normal
							vertices[1].push_back(blockNormals[3 * side + 0]);
							vertices[1].push_back(blockNormals[3 * side + 1]);
							vertices[1].push_back(blockNormals[3 * side + 2]);
						}
						// Add indices
						for (int j = 0; j < 6; ++j) {
							indices[1].push_back(faceIndices[j] + index_offset[1]);
						}
						index_offset[1] += 4;
					}
				} else { // Create chunk mesh
					// Non-water
					if (blockData[neighborType].isTransparent) {
						// Texture coordinates
						float textureIndex = blockData[blockType].textureIndices[side];

						// Add vertices
						for (int j = 0; j < 4; ++j) {
							// Position
							vertices[0].push_back(blockVertices[12 * side + 3 * j + 0] + offset.x);
							vertices[0].push_back(blockVertices[12 * side + 3 * j + 1] + offset.y);
							vertices[0].push_back(blockVertices[12 * side + 3 * j + 2] + offset.z);
							// Texture coords
							vertices[0].push_back(faceTextureCoordinates[2 * j + 0]);
							vertices[0].push_back(faceTextureCoordinates[2 * j + 1]);
							vertices[0].push_back(textureIndex);
							// Normal
							vertices[0].push_back(blockNormals[3 * side + 0]);
							vertices[0].push_back(blockNormals[3 * side + 1]);
							vertices[0].push_back(blockNormals[3 * side + 2]);
						}
						// Add indices
						for (int j = 0; j < 6; ++j) {
							indices[0].push_back(faceIndices[j] + index_offset[0]);
						}
						index_offset[0] += 4;
					}
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
	for (int i = 0; i < 2; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);

		glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(GLfloat), vertices[i].data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(GLuint), indices[i].data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		n_indices[i] = (unsigned int)indices[i].size();
	}

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
	return (glm::ivec3) glm::floor(blockPos / (float)CHUNK_SIZE);
}

bool isLegalBlockPos(const glm::ivec3& pos) {
	return pos.x >= 0 && pos.x < CHUNK_SIZE && pos.y >= 0 && pos.y < CHUNK_SIZE && pos.z >= 0 && pos.z < CHUNK_SIZE;
}

glm::ivec3 getNeighborPos(const int side, const glm::ivec3& pos) {
	glm::ivec3 neighborPos = pos;
	neighborPos[neighborIndexMap[0][side]] += neighborIndexMap[1][side];
	return neighborPos;
}
