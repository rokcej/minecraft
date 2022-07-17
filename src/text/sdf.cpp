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


void GenerateSDF(int spread, uint8_t* bitmap, int bitmap_w, int bitmap_h, uint8_t* sdf, int sdf_w, int sdf_h) {
	// Calculate size of distance transform
	float dt_spread = (float)(spread * bitmap_w) / (sdf_w - 2 * spread);
	int dt_padding = (int)std::ceil(dt_spread);
	int dt_w = bitmap_w + 2 * dt_padding;
	int dt_h = bitmap_h + 2 * dt_padding;

	// Initialize distance transform data
	std::unique_ptr<float[]> dt(new float[dt_w * dt_h]); // Distance transform of bitmap
	std::unique_ptr<float[]> dt_inv(new float[dt_w * dt_h]); // Distance transform of inverted bitmap
	for (int y = 0; y < dt_h; ++y) {
		for (int x = 0; x < dt_w; ++x) {
			int i = x + y * dt_w;
			if (x >= dt_padding && x < dt_w - dt_padding && y >= dt_padding && y < dt_h - dt_padding) {
				int j = (x - dt_padding) + (y - dt_padding) * bitmap_w;
				dt[i] = (bitmap[j] == 0) ? kInf : 0.0f;
				dt_inv[i] = (bitmap[j] == 0) ? 0.0f : kInf;
			} else {
				dt[i] = kInf;
				dt_inv[i] = 0.0f;
			}
		}
	}

	// Compute squared distances
	ApplySquaredDistanceTransform2D(dt.get(), dt_w, dt_h);
	ApplySquaredDistanceTransform2D(dt_inv.get(), dt_w, dt_h);

	// Convert distance transform to SDF
	for (int y = 0; y < sdf_h; ++y) {
		for (int x = 0; x < sdf_w; ++x) {
			// TODO: Interpolate
			int x_dt = (int)std::round((float)(x * dt_w) / sdf_w);
			int y_dt = (int)std::round((float)(y * dt_h) / sdf_h);
			int i = x_dt + y_dt * dt_w;

			// Compute normalized signed distance
			// We offset the distance by 0.5 to shift the outline from the center to the edge of a pixel
			// Example:
			//   bitmap = [0, 0, 1, 1]
			//   distance_transform = [-2, -1, +1, +1] --> [-1.5, -0.5, +0.5, +1.5]
			float sd;
			if (dt[i] <= 0.0f) {
				sd = std::sqrt(dt_inv[i]) - 0.5f;
				sd = std::min(dt_spread, sd);
			} else {
				sd = 0.5f - std::sqrt(dt[i]);
				sd = std::max(-dt_spread, sd);
			}
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

} // namespace sdf
