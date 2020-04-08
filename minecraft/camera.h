#pragma once

#include <glm/glm.hpp>
#include "player.h"

class Camera {
public:
	int renderDistance = 10;
	float fieldOfView = glm::radians(60.f); // Vertical field of view

	float aspectRatio;
	float nearDistance = 0.1f;

	bool thirdPerson = false;

	bool enableFog = true; // Enable fog at the edge of render distance
	int fogDepth = 3; // Fog depth in chunks

	glm::vec3 relativePos = { 0.f, 1.5f, 0.f };
	glm::mat4 projMat, viewMat;

	Player* entity = nullptr;

	Camera(Player* entity, float aspectRatio);

	void setRenderDistance(int renderDistance);
	void setFieldOfView(float fieldOfView);
	void setAspectRatio(float aspectRatio);
	void setNearDistance(float nearDistance);

	glm::vec3 getPos() const;
	glm::vec3 getForward() const;
	glm::vec3 getUp() const;
	glm::vec3 getRight() const;
	float getFarDistance() const;

	void updateViewMat();
	void updateProjMat();
};
