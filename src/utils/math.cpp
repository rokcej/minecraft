#include "math.h"

#include <cmath>

namespace math {

int RoundUpToMultipleOf4(float x) {
	return ((int)std::ceil(x) + 3) & ~0x3;
}

void SolveQuadraticEquation(float a, float b, float c, float* x_add, float* x_sub) {
	float sqrt_D = std::sqrt(b * b - 4.0f * a * c);
	float inv_2a = 1.0f / (2.0f * a);
	if (x_add) {
		*x_add = (-b + sqrt_D) * inv_2a;
	}
	if (x_sub) {
		*x_sub = (-b - sqrt_D) * inv_2a;
	}
}

} // namespace math
