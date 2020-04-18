#pragma once

#include <glm/glm.hpp>
#include "model.h"

class Player {
public:
	float speed = 3.f * 1.5f;

	glm::vec3 pos{ 0.f, 0.f, 0.f }; // x, y, z
	glm::vec2 rot{ 0.f, 0.f }; // yaw, pitch

	glm::vec3 forward{ 0.f, 0.f, -1.f };
	glm::vec3 up{ 0.f, 1.f, 0.f };
	glm::vec3 right{ 1.f, 0.f, 0.f };

	glm::vec3 move{ 0.f, 0.f, 0.f };

	Model model;

	Player();

	void update(float dt);
	void setPos(const glm::vec3& newPos);
};
