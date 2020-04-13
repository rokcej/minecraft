#include "hud_renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include "shader_loader.h"
#include "texture_loader.h"



#include "perlin_noise.h"
#include <iostream>
bool drawNoise = false;


float crosshairSize = 0.02f;
GLfloat crosshairVertices[] = {
	-crosshairSize, -crosshairSize, 0.f, 0.f,
	+crosshairSize, -crosshairSize, 1.f, 0.f,
	-crosshairSize, +crosshairSize, 0.f, 1.f,
	+crosshairSize, +crosshairSize, 1.f, 1.f,
};

GLfloat fullscreenVertices[] = {
	-1.f, -1.f, 0.f, 0.f,
	+1.f, -1.f, 1.f, 0.f,
	-1.f, +1.f, 0.f, 1.f,
	+1.f, +1.f, 1.f, 1.f,
};

HUDRenderer::HUDRenderer() {
	progGUI = ShaderLoader::compileProgram("gui_vertex.glsl", "gui_fragment.glsl");

	texCrosshair = TextureLoader::loadTexture("crosshair.png");

	glGenVertexArrays(1, &vaoCrosshair);
	glGenBuffers(1, &vboCrosshair);

	glBindVertexArray(vaoCrosshair);
	glBindBuffer(GL_ARRAY_BUFFER, vboCrosshair);

	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

HUDRenderer::~HUDRenderer() {
	glDeleteProgram(progGUI);
	glDeleteTextures(1, &texCrosshair);


	glDeleteBuffers(1, &vboCrosshair);
	glDeleteVertexArrays(1, &vaoCrosshair);
}

void HUDRenderer::render(float aspectRatio) {
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texCrosshair);

	glUseProgram(progGUI);
	glUniform1i(glGetUniformLocation(progGUI, "uTex"), 0);
	glUniform1f(glGetUniformLocation(progGUI, "uInvAspectRatio"), 1.f / aspectRatio);

	glBindVertexArray(vaoCrosshair);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_DEPTH_TEST);
}