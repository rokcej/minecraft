#include "collision.h"

#include <math.h>
#include <iostream>

#define TOLERANCE 0.0001f

// p- and n-vertex lookup table
// For p-vertex lookup create a bitfield of plane normal component signs, where x is LSB and z is MSB, to get the index
// For n-vertex lookup invert the index bits (reverse the order)
int p_n_vertexLUT[] = {
	///// x, y, z sign
	0, // -, -, -
	1, // +, -, -
	2, // -, +, -
	3, // +, +, -
	4, // -, -, +
	5, // +, -, +
	6, // -, +, +
	7, // +, +, +
};

// Create a plane from normal vector and point on the plane
Plane::Plane(const glm::vec3& normal, const glm::vec3& point) :
	normal(normal),
	constant{ glm::dot(normal, point) }
{}
// Signed distance of point from plane
float Plane::distance(const glm::vec3& point) {
	return glm::dot(this->normal, point) - this->constant;
}

// Create viewing frustum from camera
Frustum::Frustum(const Camera& cam) {
	// Vectors
	glm::vec3 pos = cam.getPos();
	glm::vec3 forward = cam.getForward();
	glm::vec3 right = cam.getRight();
	glm::vec3 up = cam.getUp();
	// Parameters
	float nh = 2.f * tanf(cam.fieldOfView * 0.5f) * cam.nearDistance; // Near height
	float nw = nh * cam.aspectRatio; // Near width

	// Far
	glm::vec3 relFarCenter = forward * cam.getFarDistance(); // Center of far plane relative to the camera
	planes[Side::BACK] = Plane(-forward, pos + relFarCenter);

	// Near
	glm::vec3 relNearCenter = forward * cam.nearDistance; // Center of near plane relative to the camera
	planes[Side::FRONT] = Plane(forward, pos + relNearCenter);

	// Left
	glm::vec3 leftNorm = glm::normalize(glm::cross(relNearCenter - 0.5f * nw * right, up));
	planes[Side::LEFT] = Plane(leftNorm, pos);

	// Right
	glm::vec3 rightNorm = glm::normalize(glm::cross(up, relNearCenter + 0.5f * nw * right));
	planes[Side::RIGHT] = Plane(rightNorm, pos);

	// Bottom
	glm::vec3 botNorm = glm::normalize(glm::cross(right, relNearCenter - 0.5f * nh * up));
	planes[Side::BOT] = Plane(botNorm, pos);

	// Top
	glm::vec3 topNorm = glm::normalize(glm::cross(relNearCenter + 0.5f * nh * up, right));
	planes[Side::TOP] = Plane(topNorm, pos);

	// Get p- and n-vertex indices
	for (int side = 0; side < 6; ++side) {
		int pIndex = 0;
		if (planes[side].normal.x > 0) pIndex |= 1;
		if (planes[side].normal.y > 0) pIndex |= 1 << 1;
		if (planes[side].normal.z > 0) pIndex |= 1 << 2;

		pIndices[side] = p_n_vertexLUT[pIndex];

		//int nIndex = ~pIndex & 0b111;
		//nIndices[side] = p_n_vertexLUT[nIndex];
	}
}

// Returns true if AABB is either completely inside the frustum or intersects it
bool Frustum::intersects(const AABB& aabb) {
	for (int side = 0; side < 6; ++side) { // Frustum planes
		// Test p-vertex
		if (planes[side].distance(aabb.points[pIndices[side]]) + TOLERANCE < 0)
			return false;
		//// Test n-vertex // Not necessary if we're not distinguishing between intersecting and being completely inside
		//else if (this->planes[side].distance(aabb->points[nIndices[side]]) < 0)
		//	intersect = true;
	}
	return true;
}

// Creates axis aligned bounding box from chunk
AABB::AABB(const Chunk* c) {
	float size = (float)CHUNK_SIZE;

	// Ordered by first incrementing x, then y, then z
	points[0] = glm::vec3((float)c->pos.x, (float)c->pos.y, (float)c->pos.z) * size;
	points[1] = points[0] + glm::vec3(size, 0.0f, 0.0f);
	points[2] = points[0] + glm::vec3(0.0f, size, 0.0f);
	points[3] = points[0] + glm::vec3(size, size, 0.0f);
	points[4] = points[0] + glm::vec3(0.0f, 0.0f, size);
	points[5] = points[0] + glm::vec3(size, 0.0f, size);
	points[6] = points[0] + glm::vec3(0.0f, size, size);
	points[7] = points[0] + glm::vec3(size, size, size);
}
