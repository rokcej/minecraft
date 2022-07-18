#include "font.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <glad/glad.h>
#include <src/gl/texture.h>
#include <src/utils/rect_pack.h>
#include <src/utils/math.h>
#include <src/text/sdf.h>

const int Font::kFirstChar = 32;
const int Font::kNumChars = 128 - Font::kFirstChar;

// Generate font with specified height
Font::Font(const std::string& file_path, int font_height) {
	// TODO: Move this outside
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		std::cerr << "[ERROR] Failed to initialize FreeType library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face((FT_Library)library, file_path.c_str(), 0, &face)) {
		std::cerr << "[ERROR] Failed to load font " << file_path << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, font_height);
	//FT_Property_Set(library, "sdf", "spread", &spread); // #include <freetype/ftmodapi.h>


	// Load glyphs
	std::vector<FT_Glyph> glyphs(kNumChars);
	std::vector<rect_pack::Rectangle> rects(kNumChars);
	int rect_area_sum = 0;

	for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF)) {
			std::cerr << "[ERROR] Failed to render Glyph " << c << std::endl;
			continue;
		}
		if (FT_Get_Glyph(face->glyph, &(glyphs[c - kFirstChar]))) {
			std::cerr << "[ERROR] Failed to get Glyph " << c << std::endl;
			continue;
		}

		int width = (int)face->glyph->bitmap.width;
		int height = (int)face->glyph->bitmap.rows;
		rects[c - kFirstChar] = { c, width, height, 0, 0 };
		rect_area_sum += width * height;
	}


	// Initialize atlas
	int atlas_width = math::RoundUpToMultipleOf4(std::sqrt((float)rect_area_sum));
	int upscale_iters = 0;
	do {
		atlas_width = math::RoundUpToMultipleOf4(atlas_width * 1.05f); // Scale up by 5%
		++upscale_iters;
	} while (!rect_pack::RowPacking(atlas_width, atlas_width, rects));
	int atlas_height = atlas_width;

	// TODO: Zero-initialize texture atlas (is this even necessary?)
	atlas_ = std::make_unique<Texture>(atlas_width, atlas_height, 1, nullptr);

	float compression_ratio = (float)rect_area_sum / (atlas_width * atlas_height);
	std::cout << "[Font atlas] " << 
		"resolution="   << atlas_width << "x" << atlas_height << ", " << 
		"ratio=" << compression_ratio << ", " << 
		"iters=" << upscale_iters << std::endl;


	// Copy glyphs into atlas
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable 4-byte alignment restriction
	for (const auto& rect : rects) {
		const unsigned char c = rect.id;
		const FT_BitmapGlyph& bit_glyph = (FT_BitmapGlyph)glyphs[c - kFirstChar];

		// This is faster than creating atlas buffer on the CPU (tested using std::copy)
		atlas_->SubImage(rect.x, rect.y, rect.w, rect.h, bit_glyph->bitmap.buffer);

		const glm::vec2 atlas_size(atlas_width, atlas_height);
		const float font_size = (float)font_height;
		characters_.insert({ c, {
			glm::vec2(rect.x, rect.y) / atlas_size,
			glm::vec2(rect.x + rect.w, rect.y + rect.h) / atlas_size,
			glm::vec2(rect.w, rect.h) / font_size,
			glm::vec2(bit_glyph->left, bit_glyph->top) / font_size,
			(float)(bit_glyph->root.advance.x >> 16) / font_size
		} });

		FT_Done_Glyph(glyphs[c - kFirstChar]);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Re-enable default alignment


	FT_Done_Face(face);

	FT_Done_FreeType(library);
}



// Generate font that fits into specified atlas size
Font::Font(const std::string& file_path, int atlas_width, int atlas_height) {
	// TODO: Move this outside
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		std::cerr << "[ERROR] Failed to initialize FreeType library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face((FT_Library)library, file_path.c_str(), 0, &face)) {
		std::cerr << "[ERROR] Failed to load font " << file_path << std::endl;
	}

	const int spread = 8;

	// Find maximum font height that could fit into atlas
	int predicted_height = (int)std::sqrt((float)(atlas_width * atlas_height) / kNumChars);
	int A = 0; // A = sum_i(w_i * h_i)
	int B = 0; // B = sum_i(2 * s * (w_i + h_i))
	int C = 0; // C = sum_i(4 * s^2) - atlas_area
	FT_Set_Pixel_Sizes(face, 0, predicted_height);
	for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		int width = (int)face->glyph->bitmap.width;
		int height = (int)face->glyph->bitmap.rows;
		if (width > 0 && height > 0) {
			A += width * height;
			B += width + height;
			C += 1;
		}
	}
	B = 2 * spread * B;
	C = 4 * spread * spread * C - (atlas_width * atlas_height);
	float scale_predicted_height;
	math::SolveQuadraticEquation((float)A, (float)B, (float)C, &scale_predicted_height, nullptr);
	int candidate_height = (int)(predicted_height * scale_predicted_height);

	// Downscale height until font fits into atlas
	std::vector<rect_pack::Rectangle> rects(kNumChars);
	int downscale_iters = 0;
	while (true) {
		FT_Set_Pixel_Sizes(face, 0, candidate_height);
		for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
			if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
				std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
				continue;
			}
			int width = (int)face->glyph->bitmap.width;
			int height = (int)face->glyph->bitmap.rows;
			if (width > 0 && height > 0) {
				width += 2 * spread;
				height += 2 * spread;
			}
			rects[c - kFirstChar] = { c, width, height, 0, 0 };
		}
		if (rect_pack::RowPacking(atlas_width, atlas_width, rects)) {
			break;
		}
		candidate_height = (int)(candidate_height * 0.95);
		++downscale_iters;
	}
	int final_height = candidate_height;

	// TODO: Zero-initialize texture atlas (is this even necessary?)
	atlas_ = std::make_unique<Texture>(atlas_width, atlas_height, 1, nullptr);

	// Load glyphs and copy them into atlas
	int glyph_area_sum = 0;
	FT_Set_Pixel_Sizes(face, 0, final_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable 4-byte alignment restriction
	for (const auto& rect : rects) {
		const unsigned char c = rect.id;

		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF)) {
			std::cerr << "[ERROR] Failed to render Glyph " << c << std::endl;
			continue;
		}

		int width = (int)face->glyph->bitmap.width;
		int height = (int)face->glyph->bitmap.rows;
		glyph_area_sum += width * height;

		// This is faster than creating atlas buffer on the CPU (tested using std::copy)
		atlas_->SubImage(rect.x, rect.y, rect.w, rect.h, face->glyph->bitmap.buffer);

		const glm::vec2 atlas_size(atlas_width, atlas_height);
		const float font_size = (float)final_height;
		characters_.insert({ c, {
			glm::vec2(rect.x, rect.y) / atlas_size,
			glm::vec2(rect.x + rect.w, rect.y + rect.h) / atlas_size,
			glm::vec2(rect.w, rect.h) / font_size,
			glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top) / font_size,
			(float)(face->glyph->advance.x >> 6) / font_size
		} });
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Re-enable default alignment


	float compression_ratio = (float)glyph_area_sum / (atlas_width * atlas_height);
	std::cout << "[Font atlas] " <<
		"resolution=" << atlas_width << "x" << atlas_height << ", " <<
		"ratio=" << compression_ratio << ", " <<
		"iters=" << downscale_iters << std::endl;


	FT_Done_Face(face);

	FT_Done_FreeType(library);
}



// Generate font that fits into specified atlas size
Font::Font(const std::string& file_path, int atlas_width, int atlas_height, int spread) {
	// TODO: Move this outside
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		std::cerr << "[ERROR] Failed to initialize FreeType library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face((FT_Library)library, file_path.c_str(), 0, &face)) {
		std::cerr << "[ERROR] Failed to load font " << file_path << std::endl;
	}

	// Find maximum font height that could fit into atlas
	int predicted_height = (int)std::sqrt((float)(atlas_width * atlas_height) / kNumChars);
	int A = 0; // A = sum_i(w_i * h_i)
	int B = 0; // B = sum_i(2 * s * (w_i + h_i))
	int C = 0; // C = sum_i(4 * s^2) - atlas_area
	FT_Set_Pixel_Sizes(face, 0, predicted_height);
	for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		int width = (int)face->glyph->bitmap.width;
		int height = (int)face->glyph->bitmap.rows;
		if (width > 0 && height > 0) {
			A += width * height;
			B += width + height;
			C += 1;
		}
	}
	B = 2 * spread * B;
	C = 4 * spread * spread * C - (atlas_width * atlas_height);
	float scale_predicted_height;
	math::SolveQuadraticEquation((float)A, (float)B, (float)C, &scale_predicted_height, nullptr);
	int candidate_height = (int)(predicted_height * scale_predicted_height);

	// Downscale height until font fits into atlas
	std::vector<rect_pack::Rectangle> rects(kNumChars);
	int downscale_iters = 0;
	while (true) {
		FT_Set_Pixel_Sizes(face, 0, candidate_height);
		for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
			if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
				std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
				continue;
			}
			int width = (int)face->glyph->bitmap.width;
			int height = (int)face->glyph->bitmap.rows;
			if (width > 0 && height > 0) {
				width += 2 * spread;
				height += 2 * spread;
			}
			rects[c - kFirstChar] = { c, width, height, 0, 0 };
		}
		if (rect_pack::RowPacking(atlas_width, atlas_width, rects)) {
			break;
		}
		candidate_height = (int)(candidate_height * 0.95);
		++downscale_iters;
	}
	int final_height = candidate_height;

	// TODO: Zero-initialize texture atlas (is this even necessary?)
	atlas_ = std::make_unique<Texture>(atlas_width, atlas_height, 1, nullptr);

	// Load glyphs and copy them into atlas
	int glyph_area_sum = 0;
	int render_height = 2048;
	FT_Set_Pixel_Sizes(face, 0, render_height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable 4-byte alignment restriction
	for (const auto& rect : rects) {
		const unsigned char c = rect.id;

		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
			std::cerr << "[ERROR] Failed to render Glyph " << c << std::endl;
			continue;
		}

		glyph_area_sum += rect.w * rect.h;

		std::unique_ptr<uint8_t[]> sdf_data(new uint8_t[rect.w * rect.h]);
		const FT_Bitmap& bitmap = face->glyph->bitmap; // Alias for readability
		sdf::GenerateSDF(spread, bitmap.buffer, bitmap.width, bitmap.rows, sdf_data.get(), rect.w, rect.h);

		// This is faster than creating atlas buffer on the CPU (tested using std::copy)
		atlas_->SubImage(rect.x, rect.y, rect.w, rect.h, sdf_data.get());

		const glm::vec2 atlas_size(atlas_width, atlas_height);
		const float font_size = (float)final_height;
		characters_.insert({ c, {
			glm::vec2(rect.x, rect.y) / atlas_size,
			glm::vec2(rect.x + rect.w, rect.y + rect.h) / atlas_size,
			glm::vec2(rect.w, rect.h) / font_size,
			glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top) / (float)render_height,
			(float)(face->glyph->advance.x >> 6) / (float)render_height
		} });
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Re-enable default alignment

	float compression_ratio = (float)glyph_area_sum / (atlas_width * atlas_height);
	std::cout << "[Font atlas] " <<
		"resolution=" << atlas_width << "x" << atlas_height << ", " <<
		"ratio=" << compression_ratio << ", " <<
		"iters=" << downscale_iters << std::endl;


	FT_Done_Face(face);

	FT_Done_FreeType(library);
}



Font::~Font() {

}

const FontCharacter& Font::GetCharacter(char c) const {
	return characters_.at(c);
}

const std::unique_ptr<Texture>& Font::GetAtlas() const {
	return atlas_;
}


//FontCharacter::FontCharacter(glm::vec2 uv_min, glm::vec2 uv_max, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance) {
//	uv_min_ = uv_min;
//	uv_max_ = uv_max;
//	size_ = size;
//	bearing_ = bearing;
//	advance_ = advance;
//}
