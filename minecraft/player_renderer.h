#pragma once

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera;
class Player;

class PlayerRenderer {
public:
	GLuint progOutline = 0;
	glm::mat4 transformMat;

	PlayerRenderer();
	~PlayerRenderer();

	void render(const Camera& camera, const Player& selection);
};

