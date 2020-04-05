#include "chunk_renderer.h"

#include <math.h>
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

	// Fog
	glUniform1i(glGetUniformLocation(prog, "uEnableFog"), (int)camera->enableFog);
	glUniform3fv(glGetUniformLocation(prog, "uCameraPos"), 1, glm::value_ptr(camera->getPos()));
	glUniform1f(glGetUniformLocation(prog, "uRenderDistance"), (float)camera->renderDistance * CHUNK_SIZE);
	glUniform1f(glGetUniformLocation(prog, "uFogDepth"), (float)camera->fogDepth * CHUNK_SIZE);

	glm::ivec3 chunkPos = blockToChunkPos(camera->getPos());

	// Render chunks in sphere radius
	float radius2 = (camera->renderDistance + 0.5f) * (camera->renderDistance + 0.5f) + 0.5f * 0.5f;
	int xRange = camera->renderDistance;
	for (int x = -xRange; x <= xRange; ++x) {
		float xMin = 0.f;
		if (x < 0) xMin = x + 0.5f;
		else if (x > 0) xMin = x - 0.5f;
		

		int yRange = (int)(sqrtf(radius2 - xMin * xMin) + 0.5f);
		for (int y = -yRange; y <= yRange; ++y) {
			float yMin = 0.f;
			if (y < 0) yMin = y + 0.5f;
			else if (y > 0) yMin = y - 0.5f;

			int zRange = (int)(sqrtf(radius2 - xMin * xMin - yMin * yMin) + 0.5f);
			for (int z = -zRange; z <= zRange; ++z) {
				Chunk* c = chunkManager->getChunk(x + chunkPos.x, y + chunkPos.y, z + chunkPos.z);
				if (c != nullptr && c->meshLoaded && c->n_indices > 0) {
					glBindVertexArray(c->vao);
					glDrawElements(GL_TRIANGLES, c->n_indices, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}

	// Render chunks in square radius
	/*for (int x = chunkPos.x - camera->renderDistance; x <= chunkPos.x + camera->renderDistance; ++x) {
		for (int y = chunkPos.y - camera->renderDistance; y <= chunkPos.y + camera->renderDistance; ++y) {
			for (int z = chunkPos.z - camera->renderDistance; z <= chunkPos.z + camera->renderDistance; ++z) {
				Chunk* c = chunkManager->getChunk(x, y, z);
				if (c != nullptr && c->meshLoaded && c->n_indices > 0) {
					glBindVertexArray(c->vao);
					glDrawElements(GL_TRIANGLES, c->n_indices, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}*/
}
