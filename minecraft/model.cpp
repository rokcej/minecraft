#include "model.h"

#include <iostream>

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

namespace Meshes {
	Mesh* blockOutline;
};

bool meshDataInitialized = false;

void initMeshData() {
	if (meshDataInitialized) {
		std::cout << "Block data already initialized!" << std::endl;
		return;
	}
	meshDataInitialized = true;


	// Block outline
	Meshes::blockOutline = new Mesh();
	glBindVertexArray(Meshes::blockOutline->vao);
	glBindBuffer(GL_ARRAY_BUFFER, Meshes::blockOutline->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Meshes::blockOutline->ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(indexedBlockVertices), indexedBlockVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(blockLineIndices), blockLineIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Meshes::blockOutline->n_indices = sizeof(blockLineIndices) / sizeof(GLuint);
}

Mesh::Mesh() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

Model::Model(Mesh* mesh) : mesh{ mesh } {

}

Model::~Model() {

}
