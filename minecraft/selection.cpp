#include "selection.h"

#include <limits>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "block_data.h"
#include "camera.h"
#include "chunk_manager.h"

Selection::Selection() : selectedBlock(0), buildingBlock(0), blockModel() {
	transMat = glm::mat4(1.f);
}

void Selection::update(const Camera& camera, const ChunkManager& cm) {
	// No block selected initally
	this->isBlockSelected = false;
	this->isBuildingBlockSelected = false;

	// Vectors and scalars
	glm::vec3 pos = camera.getPos();
	glm::vec3 dir = camera.getForward();
	float distance = 0.f;
	glm::ivec3 intPos = glm::floor(pos);
	glm::ivec3 intPosPrev = intPos;

	// Check for blocks that intersect with direction until reaching max distance
	while (distance < this->reachDistance) {
		// Check current block
		int blockType = cm.getBlock(intPos);
		if (blockType != BlockType::AIR && blockType != BlockType::WATER) {
			this->isBlockSelected = true;
			this->selectedBlock = intPos;
			if (intPosPrev != intPos) {
				this->isBuildingBlockSelected = true;
				this->buildingBlock = intPosPrev;
			}

			this->transMat = glm::translate(glm::mat4(1.f), (glm::vec3)intPos);
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
