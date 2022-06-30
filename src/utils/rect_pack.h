#pragma once

#include <vector>

namespace rect_pack {

struct Rectangle {
	int id;
	int w, h;
	int x, y;
};

bool RowPacking(int width, int height, std::vector<Rectangle>& rects);

} // namespace rect_pack
