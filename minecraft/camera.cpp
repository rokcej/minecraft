#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "chunk.h"

Camera::Camera(Player* entity) : viewMat(1.f), projMat(1.f), entity{ entity } {

}

inline glm::vec3 Camera::getPos() {
	return entity->pos + this->relativePos;
}

void Camera::updateViewMat() {
	viewMat = glm::lookAt(
		this->getPos(),
		this->getPos() + entity->forward,
		entity->up
	);
}

void Camera::updateProjMat(float aspectRatio) {
	projMat = glm::perspective(fieldOfView, aspectRatio, 0.1f, (float)((renderDistance + 1) * CHUNK_SIZE) * sqrtf(3));
}
