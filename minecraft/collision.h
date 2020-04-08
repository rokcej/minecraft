#pragma once


#include <glm/glm.hpp>
#include "chunk.h"
#include "camera.h"
#include "defines.h"

class AABB {
public:
	glm::vec3 points[8];

	AABB(const Chunk* c);
};

class Plane {
public:
	float constant = 0.f;
	glm::vec3 normal = glm::vec3(0.f);

	Plane() {}
	Plane(const glm::vec3& normal, const glm::vec3& point);

	float distance(const glm::vec3& point);
};

// Paper or AABB frustum culling http://www.cse.chalmers.se/~uffe/vfc_bbox.pdf
class Frustum {
public:
	Frustum(const Camera& cam);

	bool intersects(const AABB& aabb);

private:
	Plane planes[6];
	int pIndices[6] = { 0 }; // (p-vertices) Points with the greatest signed distance from each plane
	//int nIndices[6] = { 0 }; // (n-vertices) Points diagonally opposite of p-vertices
};
