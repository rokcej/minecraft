#pragma once

#include <GL/glew.h>

class Model {
public:
	GLuint vao = 0;
	unsigned int n_indices;

	Model();
	~Model();

private:
	GLuint vbo = 0;
	GLuint ebo = 0;
};
