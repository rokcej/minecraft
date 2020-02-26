#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>

class Player {
public:
	float fov = (float)M_PI / 3.0f;
	float mouseSensitivity = 0.001f;
	float speed = 3 * 1.5f;

	glm::vec3 pos{ 0.0f, 0.0f, 20.0f }; // x, y, z
	glm::vec2 rot{ 0.0f, 0.0f }; // yaw, pitch

	glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	glm::vec3 move{ 0.0f, 0.0f, 0.0f };
};
