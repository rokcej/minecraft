#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(Player* entity) : viewMat(1.f), projMat(1.f), entity{ entity } {

}

void Camera::updateViewMat() {
	viewMat = glm::lookAt(
		entity->pos + this->pos,
		entity->pos + this->pos + entity->forward,
		entity->up
	);
}

void Camera::updateProjMat(float aspectRatio) {
	projMat = glm::perspective(fieldOfView, aspectRatio, 0.1f, 1000.0f);
}
