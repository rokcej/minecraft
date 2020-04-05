#pragma once

#include <glm/glm.hpp>
#include "player.h"

class Camera {
public:
	int renderDistance = 12;
	float fieldOfView = glm::radians(60.f);

	glm::vec3 pos = { 0.f, 1.5f, 0.f };
	glm::mat4 projMat, viewMat;

	Player* entity = nullptr;

	Camera(Player* entity);

	void updateViewMat();
	void updateProjMat(float aspectRatio);
};
