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

const int Font::kFirstChar = 32;
const int Font::kNumChars = 128 - Font::kFirstChar;

Font::Font(const std::string& file_path, int height) {
	// TODO: Move this outside
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face((FT_Library)library, "data/fonts/Roboto-Regular.ttf", 0, &face)) {
		std::cerr << "[ERROR] Failed to load font " << file_path << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, height);
	

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
		const float font_size = (float)height;
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
