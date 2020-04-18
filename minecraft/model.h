#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh {
public:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	unsigned int n_indices = 0;

	Mesh();
	~Mesh();
};

class Model {
public:
	Mesh* mesh = nullptr;
	glm::mat4 modelMat = glm::mat4(1.f);

	Model(Mesh* mesh);
	~Model();
};

namespace Meshes {
	extern Mesh* blockOutline;
};

void initMeshData();
