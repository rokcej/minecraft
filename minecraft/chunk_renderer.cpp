#include "chunk_renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include "texture_loader.h"

ChunkRenderer::ChunkRenderer() {
	// Shaders
	this->prog = compileProgram(vsSourceTex, fsSourceTex);

	// Textures
	this->tex = TextureLoader::loadTextureAtlas("textures.png");
}

ChunkRenderer::~ChunkRenderer() {
	glDeleteProgram(prog);
	glDeleteTextures(1, &tex);
}

void ChunkRenderer::render(Camera* camera, ChunkManager* chunkManager) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);

	glUseProgram(prog);
	glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
	glm::mat4 pvmMat = camera->projMat * camera->viewMat;
	glUniformMatrix4fv(glGetUniformLocation(prog, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));

	glm::ivec3 chunkPos = blockToChunkPos(camera->entity->pos);

	for (int x = chunkPos.x - camera->renderDistance; x <= chunkPos.x + camera->renderDistance; ++x) {
		for (int y = chunkPos.y - camera->renderDistance; y <= chunkPos.y + camera->renderDistance; ++y) {
			for (int z = chunkPos.z - camera->renderDistance; z <= chunkPos.z + camera->renderDistance; ++z) {
				Chunk* c = chunkManager->getChunk(x, y, z);
				if (c != nullptr && c->meshLoaded && c->n_indices > 0) {
					glBindVertexArray(c->vao);
					glDrawElements(GL_TRIANGLES, c->n_indices, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}
}
