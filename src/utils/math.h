#pragma once

namespace math {

// Only create one instance of each global variable
// - C++17: inline constexpr
// - Before C++17: extern const (has to be defined in a .cpp file)

inline constexpr float kPi     = 3.14159265358979323846f;
inline constexpr float kPiHalf = 1.57079632679489661923f;

int RoundUpToMultipleOf4(float x);
void SolveQuadraticEquation(float a, float b, float c, float* x_add, float* x_sub);
float MapRange(float val, float in_min, float in_max, float out_min, float out_max);

} // namespace math
