#pragma once

#include <glm/glm.hpp>

class Player {
public:
	float fov = 60.0f;
	float speed = 1.0f;

	glm::vec3 pos{ 0.0f, 0.0f, -3.0f };
	glm::vec3 dir{ 0.0f, 0.0f, 1.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	glm::vec3 move{ 0.0f, 0.0f, 0.0f };
};
