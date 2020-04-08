#include "chunk_renderer.h"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include "shader_loader.h"
#include "texture_loader.h"
#include "texture_loader.h"
#include "collision.h"

ChunkRenderer::ChunkRenderer() {
	// Shaders
	this->prog = compileProgram("chunk_vertex.glsl", "chunk_fragment.glsl");
	// Textures
	this->tex = TextureLoader::loadTextureAtlas("textures.png");
}

ChunkRenderer::~ChunkRenderer() {
	glDeleteProgram(prog);
	glDeleteTextures(1, &tex);
}

void ChunkRenderer::render(const Camera& camera, const ChunkManager& chunkManager) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);

	glUseProgram(prog);
	glUniform1i(glGetUniformLocation(prog, "uTex"), 0);
	glm::mat4 pvmMat = camera.projMat * camera.viewMat;
	glUniformMatrix4fv(glGetUniformLocation(prog, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));

	// Fog
	glUniform1i(glGetUniformLocation(prog, "uEnableFog"), (int)camera.enableFog);
	glUniform3fv(glGetUniformLocation(prog, "uCameraPos"), 1, glm::value_ptr(camera.getPos()));
	glUniform1f(glGetUniformLocation(prog, "uRenderDistance"), (float)camera.renderDistance * CHUNK_SIZE);
	glUniform1f(glGetUniformLocation(prog, "uFogDepth"), (float)camera.fogDepth * CHUNK_SIZE);

	// View frustum
	Frustum frustum(camera);
	
	// Player position in chunk coordinates
	glm::ivec3 chunkPos = blockToChunkPos(camera.getPos());

	// Render chunks in sphere radius
	float radius2 = (camera.renderDistance + 0.5f) * (camera.renderDistance + 0.5f) + 0.5f * 0.5f;
	int xRange = camera.renderDistance;
	// x
	for (int xRel = -xRange; xRel <= xRange; ++xRel) { 
		int x = xRel + chunkPos.x;
		float xMin = 0.f;
		if (xRel < 0) xMin = xRel + 0.5f;
		else if (xRel > 0) xMin = xRel - 0.5f; 
		// y
		int yRange = (int)(sqrtf(radius2 - xMin * xMin) + 0.5f);
		for (int yRel = -yRange; yRel <= yRange; ++yRel) {
			int y = yRel + chunkPos.y;
			float yMin = 0.f;
			if (yRel < 0) yMin = yRel + 0.5f;
			else if (yRel > 0) yMin = yRel - 0.5f;
			// z
			int zRange = (int)(sqrtf(radius2 - xMin * xMin - yMin * yMin) + 0.5f);
			for (int zRel = -zRange; zRel <= zRange; ++zRel) {
				int z = zRel + chunkPos.z;

				// Loop body
				Chunk* c = chunkManager.getChunk(x, y, z);
				if (c != nullptr && c->meshLoaded && c->n_indices > 0) {
					// Frustum culling
					AABB aabb(c);
					if (frustum.intersects(aabb)) {
						glBindVertexArray(c->vao);
						glDrawElements(GL_TRIANGLES, c->n_indices, GL_UNSIGNED_INT, 0);
					}
				}
			}
		}
	}
	glBindVertexArray(0); // Unbind VAO

	// Render chunks in square radius
	//for (int x = chunkPos.x - camera->renderDistance; x <= chunkPos.x + camera->renderDistance; ++x) {
	//	for (int y = chunkPos.y - camera->renderDistance; y <= chunkPos.y + camera->renderDistance; ++y) {
	//		for (int z = chunkPos.z - camera->renderDistance; z <= chunkPos.z + camera->renderDistance; ++z) {
	//			Chunk* c = chunkManager->getChunk(x, y, z);
	//			if (c != nullptr && c->meshLoaded && c->n_indices > 0) {
	//				if (frustumCulling) {
	//					AABB aabb(c);
	//					if (!frustum.intersects(&aabb))
	//						continue;
	//				}
	//				glBindVertexArray(c->vao);
	//				glDrawElements(GL_TRIANGLES, c->n_indices, GL_UNSIGNED_INT, 0);
	//			}
	//		}
	//	}
	//}
	//glBindVertexArray(0); // Unbind VAO
}
