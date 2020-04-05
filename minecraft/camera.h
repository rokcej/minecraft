#pragma once

#include <glm/glm.hpp>
#include "player.h"

class Camera {
public:
	int renderDistance = 10;
	float fieldOfView = glm::radians(60.f); // Vertical field of view

	bool enableFog = true; // Enable fog at the edge of render distance
	int fogDepth = 3; // Fog depth in chunks

	glm::vec3 relativePos = { 0.f, 1.5f, 0.f };
	glm::mat4 projMat, viewMat;

	Player* entity = nullptr;

	Camera(Player* entity);

	glm::vec3 getPos();

	void updateViewMat();
	void updateProjMat(float aspectRatio);
};
