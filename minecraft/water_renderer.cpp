#include "water_renderer.h"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "chunk.h"
#include "chunk_manager.h"
#include "shader_loader.h"
#include "texture_loader.h"

WaterRenderer::WaterRenderer() {
	// Shaders
	this->progWater = ShaderLoader::compileProgram("chunk_vertex.glsl", "chunk_fragment.glsl");
	// Textures
	this->texWater = TextureLoader::loadTextureAtlas("terrain.png");
}

WaterRenderer::~WaterRenderer() {
	glDeleteProgram(progWater);
	glDeleteTextures(1, &texWater);
}

void WaterRenderer::render(const Camera& camera, const ChunkManager& chunkManager) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texWater);

	glUseProgram(progWater);
	glUniform1i(glGetUniformLocation(progWater, "uTex"), 0);
	glm::mat4 pvMat = camera.projMat * camera.viewMat;
	glUniformMatrix4fv(glGetUniformLocation(progWater, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvMat));

	// Fog
	glUniform1i(glGetUniformLocation(progWater, "uEnableFog"), (int)camera.enableFog);
	glUniform3fv(glGetUniformLocation(progWater, "uCameraPos"), 1, glm::value_ptr(camera.getPos()));
	glUniform1f(glGetUniformLocation(progWater, "uRenderDistance"), (float)camera.renderDistance * CHUNK_SIZE);
	glUniform1f(glGetUniformLocation(progWater, "uFogDepth"), (float)camera.fogDepth * CHUNK_SIZE);

	// Render chunks in render queue
	for (const Chunk* c : chunkManager.renderList) {
		if (c->n_indices[1] > 0) {
			glBindVertexArray(c->vao[1]);
			glDrawElements(GL_TRIANGLES, c->n_indices[1], GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(0); // Unbind VAO
}
