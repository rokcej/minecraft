#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "chunk.h"
#include "player.h"

Camera::Camera(Player* entity, float aspectRatio) :
	viewMat(1.f),
	projMat(1.f),
	entity{ entity },
	aspectRatio{ aspectRatio }
{
	setRenderDistance(renderDistance);
	updateViewMat();
	updateProjMat();
}

void Camera::setRenderDistance(int renderDistance) {
	this->renderDistance = renderDistance;
	this->farDistance = (float)((renderDistance + 1) * CHUNK_SIZE) * sqrtf(3);
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
glm::vec3 Camera::getUp() const { return this->entity->up; }
glm::vec3 Camera::getRight() const { return this->entity->right; }

float Camera::getFarDistance() const {
	return this->farDistance;
}

void Camera::updateViewMat() {
	if (thirdPerson) {
		viewMat = glm::lookAt(
			getPos() - getForward() * 64.f,
			getPos() + getForward(),
			getUp()
		);
	} else {
		viewMat = glm::lookAt(
			getPos(),
			getPos() + getForward(),
			getUp()
		);
	}
}

void Camera::updateProjMat() {
	projMat = glm::perspective(fieldOfView, aspectRatio, nearDistance, getFarDistance());
}
