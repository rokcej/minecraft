#include "perlin_noise.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "hash.h"

// If using unit length gradients, noise will produce output in range [-sqrt(n)/2, sqrt(n)/2] where n is the number of dimensions
// We can change the output range to [-1, 1] by multiplying either output values OR gradients by 2/sqrt(n)
constexpr float noise2DAdjust = 2.f * (float)M_SQRT1_2;

// Smooth transition on interval x = [0, 1], has zero first and second derivative at x = 0 and x = 1
float fade(const float x) {
	return x * x * x * (x * (x * 6.f - 15.f) + 10.f); // 6x^5 - 15x^4 + 10x^3
}

// Linear interpolation
float lerp(const float x0, const float x1, const float t) {
	//return x0 + t * (x1 - x0); // Imprecise method, doesn't guarantee x = x1 when t = 1 because of float representation errors
	return (1.f - t) * x0 + t * x1; // Precise method
}

PerlinNoise::PerlinNoise() :
	rng(),
	distribution(0.f, 2.f * (float)M_PI)
{
	
}

PerlinNoise::~PerlinNoise() {

}

// Return values from interval [0, 1]
float PerlinNoise::noise(int x, int y, int scale, int octaves) {
	float lacunarity = 2.0f;
	float persistence = 0.5f;

	float octaveAmplitude = 1.f;
	float octaveScale = (float)scale;

	float sum = 0.f;
	for (int o = 0; o < octaves; ++o) {
		sum += octaveAmplitude * _noise(x, y, octaveScale);

		octaveAmplitude *= persistence;
		octaveScale /= lacunarity;
	}
	sum *= (float)(1 << (octaves - 1)) / (float)((1 << octaves) - 1); // 2^n = 1 << n

	return (sum + 1.f) * 0.5f;
}

// Internal noise function used to compute a single octave
float PerlinNoise::_noise(int xInput, int yInput, float scale) {
	// Get grid point coordinates
	float x = (float)xInput / scale;
	float y = (float)yInput / scale;
	// Get grid unit square coordinates
	int x0 = (int)floorf(x);
	int y0 = (int)floorf(y);
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	// Get relative coordinates inside of unit square
	float dx = x - (float)x0;
	float dy = y - (float)y0;
	// Get faded relative coordinates
	float u = fade(dx);
	float v = fade(dy);
	// Get dot products for with each gradient (wYX)
	float w00 = glm::dot(getGradient(x0, y0), glm::vec2(dx, dy));
	float w01 = glm::dot(getGradient(x1, y0), glm::vec2(dx - 1.f, dy));
	float w10 = glm::dot(getGradient(x0, y1), glm::vec2(dx, dy - 1.f));
	float w11 = glm::dot(getGradient(x1, y1), glm::vec2(dx - 1.f, dy - 1.f));
	// Get value by interpolating weights
	float value = lerp(
		lerp(w00, w01, u),
		lerp(w10, w11, u),
		v
	);
	// Return value
	return value;
}

// Get gradients for points on the regular grid
glm::vec2 PerlinNoise::getGradient(int x, int y) {
	glm::ivec2 key(x, y);
	auto it = gradientCache.find(key);
	if (it == gradientCache.end()) { // Gradient not cached yet
		// Generate gradient while using coordinates as seed
		rng.seed(ivec2Hash()(glm::ivec2(x, y)));
		float angle = distribution(rng);
		glm::vec2 gradient = glm::vec2(cosf(angle), sinf(angle)) * noise2DAdjust;
		gradientCache.insert(std::make_pair(key, gradient));
		return gradient;
	} else { // Gradient already cached
		// Return cached value
		return it->second;
	}
}

void PerlinNoise::clearCache() {
	gradientCache.clear();
}
