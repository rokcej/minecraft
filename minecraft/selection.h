#pragma once

#include <glm/glm.hpp>
#include "model.h"

class ChunkManager;
class Camera;

class Selection {
public:
	float reachDistance = 5.f;

	bool isBlockSelected = false;
	bool isBuildingBlockSelected = false;
	glm::ivec3 selectedBlock;
	glm::ivec3 buildingBlock;

	Model outlineModel;

	Selection();

	void update(const Camera& camera, const ChunkManager& cm);
};
