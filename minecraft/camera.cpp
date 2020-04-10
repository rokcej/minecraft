#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include "chunk.h"
#include "block_data.h"
#include "player.h"
#include "chunk_manager.h"

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

#include <iostream>
void Camera::updateSelection(const ChunkManager& cm) {
	// No block selected initally
	this->isBlockSelected = false;

	// Vectors and scalars
	glm::vec3 pos = getPos();
	glm::vec3 dir = getForward();
	float distance = 0.f;
	glm::ivec3 intPos = glm::floor(pos);
	glm::ivec3 intPosPrev = intPos;

	// Check for blocks that intersect with direction until reaching max distance
	while (distance < this->reachDistance) {
		// Check current block
		if (cm.getBlock(intPos) != BlockType::AIR) {
			this->isBlockSelected = true;
			this->selectedBlock = intPos;
			this->buildingBlock = intPosPrev;
			break;
		}

		// Find minimum direction factor to intersect with next cube
		float minFactor = std::numeric_limits<float>::infinity();
		for (int i = 0; i < 3; ++i) {
			float factor;
			if (dir[i] > 0)
				factor = (floorf(pos[i] + 1.f) - pos[i]) / dir[i];
			else if (dir[i] < 0)
				factor = (floorf(pos[i]) - pos[i]) / dir[i];
			else
				factor = std::numeric_limits<float>::infinity();

			if (factor < minFactor)
				minFactor = factor;
		}

		// Add offset
		minFactor += 0.001f;

		// Compute next position
		pos += dir * minFactor;
		intPosPrev = intPos;
		intPos = glm::floor(pos);
		distance += minFactor;
	}
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
