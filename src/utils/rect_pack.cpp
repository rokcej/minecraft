#include "rect_pack.h"

#include <algorithm>

namespace rect_pack {

bool RowPacking(int width, int height, std::vector<Rectangle>& rects) {
	std::sort(rects.begin(), rects.end(), [](const Rectangle& lhs, const Rectangle& rhs) {
		return lhs.h > rhs.h;
	});

	int row_height = rects[0].h;
	int x_pos = 0;
	int y_pos = 0;

	for (auto& rect : rects) {
		if (rect.w > width) {
			return false;
		}
		if (x_pos + rect.w > width) {
			x_pos = 0;
			y_pos += row_height;
			row_height = rect.h;
			if (y_pos + row_height > height) {
				return false;
			}
		}
		rect.x = x_pos;
		rect.y = y_pos;
		x_pos += rect.w;
	}

	return true;
}

} // namespace rect_pack
