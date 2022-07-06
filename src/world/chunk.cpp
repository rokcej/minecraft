#include "chunk.h"

#include <vector> // TODO

// TODO: Optimize vertex loading
const glm::vec3 kCubeVertices[] = { // Block vertices
	// Left
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f },
	// Right
	{ 1.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 1.0f, 1.0f, 1.0f },
	// Bottom
	{ 1.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	// Top
	{ 0.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	// Back
	{ 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f },
	// Front
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 0.0f, 1.0f, 1.0f }
};

const glm::vec2 kQuadUVs[] = { // Block face texture coordinates
	{ 0.f, 0.f }, // Bottom left
	{ 1.f, 0.f }, // Bottom right
	{ 1.f, 1.f }, // Top right
	{ 0.f, 1.f }  // Top left
};

const GLuint kQuadIndices[] = {
	0, 1, 2, 0, 2, 3
};

const glm::ivec3 kSideToDir[] = {
	{ -1.0f,  0.0f,  0.0f }, // Left
	{ +1.0f,  0.0f,  0.0f }, // Right
	{  0.0f, -1.0f,  0.0f }, // Bottom
	{  0.0f, +1.0f,  0.0f }, // Top
	{  0.0f,  0.0f, -1.0f }, // Back
	{  0.0f,  0.0f, +1.0f }  // Front
};


Chunk::Chunk(glm::vec3 pos) {
	pos_ = pos;
	for (int i = 0; i < 4096; ++i) {
		data_[i] = 1;
	}

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);

	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	glEnableVertexAttribArray(0); // Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1); // Texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// VAOs store the following calls:
	//   -For VBOs: glVertexAttribPointer and glEnableVertexAttribArray --> we CAN unbind VBOs
	//   -For EBOs: glBindBuffer --> we CAN'T unbind EBOs
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GenerateMesh();
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
	glDeleteVertexArrays(1, &vao_);
}

void Chunk::GenerateMesh() {
	std::vector<GLfloat> vertices;
	int num_quads = 0;
	for (int x = 0; x < 16; ++x) {
		for (int y = 0; y < 16; ++y) {
			for (int z = 0; z < 16; ++z) {
				int i = x + 16 * (y + 16 * z);
				if (data_[i] == 0) {
					continue;
				}

				glm::vec3 offset(x, y, z);
				for (int side = 0; side < 6; ++side) {
					int dim = side / 2;
					int dir = (side % 2) * 2 - 1;

					glm::ivec3 neigh = glm::ivec3(x, y, z);
					neigh[dim] += dir;
					if (neigh[dim] >= 0 && neigh[dim] < 16) {
						int j = neigh.x + 16 * (neigh.y + 16 * neigh.z);
						if (data_[j] != 0) {
							continue;
						}
					}

					/*glm::ivec3 neigh = glm::ivec3(x, y, z) + kSideToDir[side];
					if (neigh.x >= 0 && neigh.x < 16 && neigh.y >= 0 && neigh.y < 16 && neigh.z >= 0 && neigh.z < 16) {
						int j = neigh.x + 16 * (neigh.y + 16 * neigh.z);
						if (data_[j] != 0) {
							continue;
						}
					}*/

					for (int corner = 0; corner < 4; ++corner) {
						glm::vec3 pos = kCubeVertices[4 * side + corner] + offset + pos_;
						glm::vec2 uv = kQuadUVs[corner];
						vertices.insert(vertices.end(), { pos.x, pos.y, pos.z });
						vertices.insert(vertices.end(), { uv.x, uv.y });
					}
					++num_quads;
				}
			}
		}
	}

	std::vector<GLuint> indices(6 * num_quads);
	for (int i = 0; i < num_quads; ++i) {
		for (int j = 0; j < 6; ++j) {
			indices[6 * i + j] = kQuadIndices[j] + 4 * i;
		}
	}
	num_indices_ = (unsigned int)indices.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

	// TODO: Experiment with GL_DYNAMIC_DRAW
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
