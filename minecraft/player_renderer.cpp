#include "player_renderer.h"

#include "outline_renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "player.h"
#include "shader_loader.h"

PlayerRenderer::PlayerRenderer() {
	this->progOutline = ShaderLoader::compileProgram("outline_vertex.glsl", "outline_fragment.glsl");

	transformMat = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(.6f, 1.8f, .6f) * .9999f), glm::vec3(-.5f, .0001f, -.5f));
}

PlayerRenderer::~PlayerRenderer() {
	glDeleteProgram(progOutline);
}

void PlayerRenderer::render(const Camera& camera, const Player& player) {
	glm::mat4 pvmMat = camera.projMat * camera.viewMat * player.model.modelMat * transformMat;

	glUseProgram(progOutline);
	glUniformMatrix4fv(glGetUniformLocation(progOutline, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));
	glBindVertexArray(player.model.mesh->vao);

	glLineWidth(1.f);
	glDrawElements(GL_LINES, player.model.mesh->n_indices, GL_UNSIGNED_INT, 0);
}
