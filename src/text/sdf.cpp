#include "sdf.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <src/utils/math.h>

// Generate SDF using distance transform in O(n) time
// - http://cs.brown.edu/people/pfelzens/papers/dt-final.pdf
// - https://prideout.net/blog/distance_fields/#marching-parabolas

namespace sdf {

// Constants
constexpr float kInf = std::numeric_limits<float>::infinity();

// Internal functions
static float GetParabolaIntersection(float px, float py, float qx, float qy);
static void ApplySquaredDistanceTransform1D(float* data, int num_elements, int stride, std::vector<float>& f, std::vector<int>& v, std::vector<float>& z);
static void ApplySquaredDistanceTransform2D(float* data, int width, int height);
static float SqrtBilinearInterpolation(float x, float y, int width, int height, float* data, float border);


void GenerateSDF(int spread, uint8_t* bitmap, int bitmap_w, int bitmap_h, uint8_t* sdf, int sdf_w, int sdf_h) {
	// Calculate size of distance transform
	float dt_spread = (float)(spread * bitmap_w) / (sdf_w - 2 * spread);
	int dt_padding = (int)std::ceil(dt_spread);
	int dt_w = bitmap_w + 2 * dt_padding;
	int dt_h = bitmap_h + 2 * dt_padding;

	// Initialize distance transform data
	std::unique_ptr<float[]> dt2(new float[dt_w * dt_h]); // Distance transform of bitmap
	std::unique_ptr<float[]> dt2_inv(new float[dt_w * dt_h]); // Distance transform of inverted bitmap
	for (int y = 0; y < dt_h; ++y) {
		for (int x = 0; x < dt_w; ++x) {
			int i = x + y * dt_w;
			if (x >= dt_padding && x < dt_w - dt_padding && y >= dt_padding && y < dt_h - dt_padding) {
				int j = (x - dt_padding) + (y - dt_padding) * bitmap_w;
				dt2[i] = (bitmap[j] == 0) ? kInf : 0.0f;
				dt2_inv[i] = (bitmap[j] == 0) ? 0.0f : kInf;
			} else {
				dt2[i] = kInf;
				dt2_inv[i] = 0.0f;
			}
		}
	}

	// Compute squared distances
	ApplySquaredDistanceTransform2D(dt2.get(), dt_w, dt_h);
	ApplySquaredDistanceTransform2D(dt2_inv.get(), dt_w, dt_h);

	// Convert distance transform to SDF
	for (int y = 0; y < sdf_h; ++y) {
		for (int x = 0; x < sdf_w; ++x) {
			float x_pos = (float)((x + 0.5f) * dt_w) / sdf_w;
			float y_pos = (float)((y + 0.5f) * dt_h) / sdf_h;

			int x_dt = (int)x_pos;
			int y_dt = (int)y_pos;
			int i = x_dt + y_dt * dt_w;

			// Compute signed distance
			// We offset the distance by 0.5 to shift the outline from the center to the edge of a pixel
			// Example:
			//   bitmap = [0, 0, 1, 1]
			//   distance_transform = [-2, -1, +1, +1] --> [-1.5, -0.5, +0.5, +1.5]
			float sd;

			// Bilinear sampling
			float dist = SqrtBilinearInterpolation(x_pos, y_pos, dt_w, dt_h, dt2.get(), dt_spread + 1.0f);
			if (dist <= 0.0f) {
				float dist_inv = SqrtBilinearInterpolation(x_pos, y_pos, dt_w, dt_h, dt2_inv.get(), 0.0f);
				sd = std::min(+dt_spread, dist_inv - 0.5f);
			} else {
				sd = std::max(-dt_spread, 0.5f - dist);
			}
			// Nearest neighbour sampling
			/*if (dt2[i] <= 0.0f) {
				sd = std::min(+dt_spread, std::sqrt(dt2_inv[i]) - 0.5f);
			} else {
				sd = std::max(-dt_spread, 0.5f - std::sqrt(dt2[i]));
			}*/

			// Store normalized signed distance
			sdf[x + y * sdf_w] = (uint8_t)std::round((sd + dt_spread) * 255.0f / (2.0f * dt_spread));
		}
	}
}

static void ApplySquaredDistanceTransform2D(float* data, int width, int height) {
	// Allocate arrays
	int max_num_elements = std::max(width, height);
	std::vector<float> f(max_num_elements); // Squared distance values (y)
	std::vector<int> v(max_num_elements); // Parabola vertex locations (x)
	std::vector<float> z(max_num_elements + 1); // Parabola boundaries (x_min, x_max)

	// Horizontal pass
	for (int y = 0; y < height; ++y) {
		ApplySquaredDistanceTransform1D(data + y * width, width, 1, f, v, z);
	}
	// Vertical pass
	for (int x = 0; x < width; ++x) {
		ApplySquaredDistanceTransform1D(data + x, height, width, f, v, z);
	}
}

static void ApplySquaredDistanceTransform1D(float* data, int num_elements, int stride, std::vector<float>& f, std::vector<int>& v, std::vector<float>& z) {
	// Initialize arrays
	for (int i = 0; i < num_elements; ++i) {
		f[i] = data[i * stride];
	}
	v[0] = 0;
	z[0] = -kInf;
	z[1] = +kInf;

	// Find lower envelope
	int k = 0;
	for (int q = 1; q < num_elements; ++q) {
		float s = GetParabolaIntersection((float)v[k], f[v[k]], (float)q, f[q]);
		while (k > 0 && s <= z[k]) {
			--k;
			s = GetParabolaIntersection((float)v[k], f[v[k]], (float)q, f[q]);
		}
		++k;
		v[k] = q;
		z[k] = s;
		z[k + 1] = +kInf;
	}

	// Evaluate lower envelope
	k = 0;
	for (int q = 0; q < num_elements; ++q) {
		while (z[k + 1] < q) {
			++k;
		}
		data[q * stride] = (q - v[k]) * (q - v[k]) + f[v[k]];
	}
}

static float GetParabolaIntersection(float px, float py, float qx, float qy) {
	// Handle edge cases with infinity
	if (py == kInf && qy == kInf) {
		return 0.5f * (px + qx);
	} else if (py == kInf) {
		return px > qx ? kInf : -kInf;
	} else if (qy == kInf) {
		return qx > px ? kInf : -kInf;
	}
	// Compute intersection
	return ((qy + qx * qx) - (py + px * px)) / (2.0f * qx - 2.0f * px);
}

static float SqrtBilinearInterpolation(float x, float y, int width, int height, float* data, float border) {
	int x0 = (int)std::floor(x - 0.5f);
	int y0 = (int)std::floor(y - 0.5f);
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	float dx = (x - 0.5f) - (float)x0;
	float dy = (y - 0.5f) - (float)y0;

	bool valid00 = x0 >= 0 && y0 >= 0;
	bool valid10 = x1 < width && y0 >=0;
	bool valid01 = x0 >= 0 && y1 < height;
	bool valid11 = x1 < width && y1 < height;

	float f00 = valid00 ? std::sqrt(data[x0 + y0 * width]) : border;
	float f10 = valid10 ? std::sqrt(data[x1 + y0 * width]) : border;
	float f01 = valid01 ? std::sqrt(data[x0 + y1 * width]) : border;
	float f11 = valid11 ? std::sqrt(data[x1 + y1 * width]) : border;

	float fx0 = f00 * (1.0f - dx) + f10 * dx;
	float fx1 = f01 * (1.0f - dx) + f11 * dx;
	return fx0 * (1.0f - dy) + fx1 * dy;
}

} // namespace sdf
