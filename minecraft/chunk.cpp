#include "chunk.h"

#include <iostream>
#include "block_data.h"

#define SIDE_LEFT 0
#define SIDE_RIGHT 1
#define SIDE_BOT 2
#define SIDE_TOP 3
#define SIDE_BACK 4
#define SIDE_FRONT 5

GLfloat blockVertices[] = {
	// Left
	0.0f, 0.0f, 0.0f,	//0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,	//1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,	//1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,	//0.0f, 1.0f,
	// Right
	1.0f, 0.0f, 1.0f,	//0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,	//1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,	//1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	//0.0f, 1.0f,
	// Bottom
	1.0f, 0.0f, 1.0f,	//0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,	//1.0f, 0.0f,
	0.0f, 0.0f, 0.0f,	//1.0f, 1.0f,
	1.0f, 0.0f, 0.0f,	//0.0f, 1.0f,
	// Top
	0.0f, 1.0f, 1.0f,	//0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	//1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,	//1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,	//0.0f, 1.0f,
	// Back
	1.0f, 0.0f, 0.0f,	//0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,	//1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,	//1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,	//0.0f, 1.0f,
	// Front
	0.0f, 0.0f, 1.0f,	//0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,	//1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	//1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,	//0.0f, 1.0f,
};

GLfloat uvOffset = 0.0001f; // To prevent texture bleeding
GLfloat blockU0 = 0.0f + uvOffset, blockU1 = 1.0f / 16.0f - uvOffset;
GLfloat blockV0 = 1.0f - 1.0f / TEXTURE_ATLAS_HEIGHT + uvOffset, blockV1 = 1.0f - uvOffset;
GLfloat blockUV[] = {
	blockU0, blockV0,
	blockU1, blockV0,
	blockU1, blockV1,
	blockU0, blockV1
};

GLuint blockIndices[] = {
	0, 1, 2,	0, 2, 3,
};


int blockNeighbours[] = {
	-1, 0, 0, +1, 0, 0, // Left, right
	0, -1, 0, 0, +1, 0, // Bottom, top
	0, 0, -1, 0, 0, +1  // Back, front
};

Chunk::Chunk(int x, int y, int z) {
	initBlockData();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	generateData();
	generateMesh();
}

Chunk::~Chunk() {}

void Chunk::generateData() {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
	for (int y = 0; y < CHUNK_SIZE; ++y) {
	for (int z = 0; z < CHUNK_SIZE; ++z) {
		if (y == CHUNK_SIZE - 1)
			data[z][y][x] = BLOCK_GRASS;
		else
			data[z][y][x] = BLOCK_DIRT;
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
			for (int side = 0; side < 6; ++side) {
				int xn = x + blockNeighbours[3 * side];
				int yn = y + blockNeighbours[3 * side + 1];
				int zn = z + blockNeighbours[3 * side + 2];
				uint8_t neighbourType = data[zn][yn][xn];

				if (!(xn >= 0 && xn < CHUNK_SIZE &&
					yn >= 0 && yn < CHUNK_SIZE &&
					zn >= 0 && zn < CHUNK_SIZE) ||
					neighbourType == BLOCK_AIR) {
					// Texture coordinates
					float u, v;
					if (side == SIDE_TOP) {
						u = blockData[blockType].uTop;
						v = blockData[blockType].vTop;
					} else if (side == SIDE_BOT) {
						u = blockData[blockType].uBot;
						v = blockData[blockType].vBot;
					} else {
						u = blockData[blockType].uSide;
						v = blockData[blockType].vSide;
					}

					// Add vertices
					for (int j = 0; j < 4; ++j) {
						vertices.push_back(blockVertices[12 * side + 3 * j + 0] + (GLfloat)x);
						vertices.push_back(blockVertices[12 * side + 3 * j + 1] + (GLfloat)y);
						vertices.push_back(blockVertices[12 * side + 3 * j + 2] + (GLfloat)z);
						vertices.push_back(blockUV[2 * j + 0] + u);
						vertices.push_back(blockUV[2 * j + 1] - v);
					}
					// Add indices
					for (int j = 0; j < 6; ++j) {
						indices.push_back(blockIndices[j] + index_offset);
					}
					index_offset += 4;
				}
			}
		}
	}
	}
	}

	// Put vertices and indices in vbo and ebo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data() , GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	n_indices = (unsigned int)indices.size();
}
