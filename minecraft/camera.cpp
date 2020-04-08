#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "chunk.h"

Camera::Camera(Player* entity, float aspectRatio) :
	viewMat(1.f),
	projMat(1.f),
	entity{ entity },
	aspectRatio{ aspectRatio }
{
	updateViewMat();
	updateProjMat();
}

void Camera::setRenderDistance(int renderDistance) {
	this->renderDistance = renderDistance;
	this->updateProjMat();
}
void Camera::setFieldOfView(float fieldOfView) {
	this->fieldOfView = fieldOfView;
	this->updateProjMat();
}
void Camera::setAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	this->updateProjMat();
}
void Camera::setNearDistance(float nearDistance) {
	this->nearDistance = nearDistance;
	this->updateProjMat();
}

glm::vec3 Camera::getPos() const { return this->entity->pos + this->relativePos; }
glm::vec3 Camera::getForward() const { return this->entity->forward;  }
glm::vec3 Camera::getUp() const { return glm::cross(this->entity->right, this->entity->forward); }
glm::vec3 Camera::getRight() const { return this->entity->right; }

float Camera::getFarDistance() const {
	return (float)((renderDistance + 1) * CHUNK_SIZE) * sqrtf(3);
}

void Camera::updateViewMat() {
	if (thirdPerson) {
		viewMat = glm::lookAt(
			this->getPos() - this->entity->forward * 64.f + this->getUp() * 0.f,
			this->getPos() + entity->forward * 0.f,
			entity->up
		);
	} else {
		viewMat = glm::lookAt(
			this->getPos(),
			this->getPos() + entity->forward,
			entity->up
		);
	}
}

void Camera::updateProjMat() {
	projMat = glm::perspective(fieldOfView, aspectRatio, nearDistance, getFarDistance());
}
