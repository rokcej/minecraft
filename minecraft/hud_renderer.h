#pragma once

#include <GL/glew.h>

class HUDRenderer {
public:
	GLuint progGUI = 0;

	GLuint texCrosshair = 0;
	GLuint vaoCrosshair = 0, vboCrosshair = 0;

	HUDRenderer();
	~HUDRenderer();

	void render(float aspectRatio);
};
