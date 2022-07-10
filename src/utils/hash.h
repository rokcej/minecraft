#pragma once

#include <glm/vec3.hpp>

namespace hash {

template<class T>
struct Hash;

// Teschner, et al. "Optimized spatial hashing for collision detection of deformable objects."
// http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf

template<typename T, glm::qualifier Q>
struct Hash<glm::vec<3, T, Q>> {

	size_t operator()(const glm::vec<3, T, Q>& v) const {
		return (73856093 * v.x) ^ (19349663 * v.y) ^ (83492791 * v.z);
	}

};

}; // namespace hash
