#include "math.h"

#include <cmath>

namespace math {

int RoundUpToMultipleOf4(float x) {
	return ((int)std::ceil(x) + 3) & ~0x3;
}

} // namespace math
