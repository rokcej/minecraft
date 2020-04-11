#pragma once

#include <glm/glm.hpp>
#include "model.h"

class ChunkManager;
class Camera;

class Selection {
public:
	bool isBlockSelected = false;
	bool isBuildingBlockSelected = false;
	glm::ivec3 selectedBlock;
	glm::ivec3 buildingBlock;

	Model blockModel;
	glm::mat4 transMat;

	Selection();

	void update(const Camera& camera, const ChunkManager& cm);
};
