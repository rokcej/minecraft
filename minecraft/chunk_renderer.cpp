#include "chunk_renderer.h"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "chunk.h"
#include "chunk_manager.h"
#include "shader_loader.h"
#include "texture_loader.h"

ChunkRenderer::ChunkRenderer() {
	// Shaders
	this->progChunk = ShaderLoader::compileProgram("chunk_vertex.glsl", "chunk_fragment.glsl");
	// Textures
	this->texChunk = TextureLoader::loadTextureAtlas("terrain.png");
}

ChunkRenderer::~ChunkRenderer() {
	glDeleteProgram(progChunk);
	glDeleteTextures(1, &texChunk);
}

void ChunkRenderer::render(const Camera& camera, const ChunkManager& chunkManager) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texChunk);

	glUseProgram(progChunk);
	glUniform1i(glGetUniformLocation(progChunk, "uTex"), 0);
	glm::mat4 pvMat = camera.projMat * camera.viewMat;
	glUniformMatrix4fv(glGetUniformLocation(progChunk, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvMat));

	// Fog
	glUniform1i(glGetUniformLocation(progChunk, "uEnableFog"), (int)camera.enableFog);
	glUniform3fv(glGetUniformLocation(progChunk, "uCameraPos"), 1, glm::value_ptr(camera.getPos()));
	glUniform1f(glGetUniformLocation(progChunk, "uRenderDistance"), (float)camera.renderDistance * CHUNK_SIZE);
	glUniform1f(glGetUniformLocation(progChunk, "uFogDepth"), (float)camera.fogDepth * CHUNK_SIZE);

	// Render chunks in render queue
	for (const Chunk* c : chunkManager.renderList) {
		if (c->n_indices[0] > 0) {
			glBindVertexArray(c->vao[0]);
			glDrawElements(GL_TRIANGLES, c->n_indices[0], GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(0); // Unbind VAO
}
