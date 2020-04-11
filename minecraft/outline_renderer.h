#pragma once

#include <GL/glew.h>

class Camera;
class Selection;

class OutlineRenderer {
public:
	float outlineSize = 0.001f;

	GLuint progOutline = 0;

	OutlineRenderer();
	~OutlineRenderer();

	void render(const Camera& camera, const Selection& selection, int height);
};
