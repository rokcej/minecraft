#pragma once

#include <cstdint>

namespace sdf {

void GenerateSDF(int spread, uint8_t* bitmap, int bitmap_w, int bitmap_h, uint8_t* sdf, int sdf_w, int sdf_h);

} // namespace sdf
