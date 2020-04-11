#include "outline_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "selection.h"
#include "shader_loader.h"

OutlineRenderer::OutlineRenderer() {
	this->progOutline = ShaderLoader::compileProgram("outline_vertex.glsl", "outline_fragment.glsl");
}

OutlineRenderer::~OutlineRenderer() {
	glDeleteProgram(progOutline);
}

void OutlineRenderer::render(const Camera& camera, const Selection& selection, int height) {
	if (selection.isBlockSelected) {
		glm::vec3 offset = -0.001f * camera.getForward();
		glm::mat4 pvmMat = camera.projMat * camera.viewMat * glm::translate(selection.transMat, offset);

		glUseProgram(progOutline);
		glUniformMatrix4fv(glGetUniformLocation(progOutline, "uPVM"), 1, GL_FALSE, glm::value_ptr(pvmMat));
		glBindVertexArray(selection.blockModel.vao);

		glLineWidth((float)height * outlineSize);
		glDrawElements(GL_LINES, selection.blockModel.n_indices, GL_UNSIGNED_INT, 0);
	}
}
