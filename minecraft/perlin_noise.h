#pragma once

#include <unordered_map>
#include <random>
#include "hash.h"

class PerlinNoise {
public:
	PerlinNoise();
	~PerlinNoise();

	float noise(int x, int y, int scale, int octaves);

	void clearCache();

private:

	std::mt19937 rng;
	std::uniform_real_distribution<float> distribution;

	std::unordered_map<glm::ivec2, glm::vec2, ivec2Hash> gradientCache;

	glm::vec2 getGradient(int x, int y);

	float _noise(int xInput, int yInput, float scale);
};
