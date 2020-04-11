#include "model.h"

GLfloat indexedBlockVertices[] {
	0.f, 0.f, 0.f,
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	1.f, 1.f, 0.f,
	0.f, 0.f, 1.f,
	1.f, 0.f, 1.f,
	0.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
};

GLuint blockIndices[] {
	0, 4, 6, 6, 2, 0, // Left
	5, 1, 3, 3, 7, 5, // Right
	0, 1, 5, 5, 4, 0, // Bottom
	6, 7, 3, 3, 2, 6, // Top
	1, 0, 2, 2, 3, 1, // Back
	4, 5, 7, 7, 6, 4  // Front
};

GLuint blockLineIndices[]{
	0, 4, 4, 6, 6, 2, 2, 0, // Left
	5, 1, 1, 3, 3, 7, 7, 5, // Right
	0, 1, 1, 5, 5, 4, 4, 0, // Bottom
	6, 7, 7, 3, 3, 2, 2, 6, // Top
	1, 0, 0, 2, 2, 3, 3, 1, // Back
	4, 5, 5, 7, 7, 6, 6, 4  // Front
};

Model::Model() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(indexedBlockVertices), indexedBlockVertices, GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(blockLineIndices), blockLineIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	n_indices = sizeof(blockLineIndices) / sizeof(GLuint);
}

Model::~Model() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}
