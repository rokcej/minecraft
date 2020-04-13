#pragma once

#include <glm/glm.hpp>
#include <cstddef> // std::size_t

// Hashing based on this paper http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf

// 3D vector hash
struct ivec3Hash {
	std::size_t operator()(const glm::ivec3& vec) const {
		return (vec.x * 11483) ^ (vec.y * 15787) ^ (vec.z * 19697);
	}
};

// 2D vector hash
struct ivec2Hash {
	std::size_t operator()(const glm::ivec2& vec) const {
		return (vec.x * 11483) ^ (vec.y * 15787);
	}
};
