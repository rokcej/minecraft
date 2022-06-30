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
	
	std::vector<FT_Glyph> glyphs(kNumChars);
	std::vector<rect_pack::Rectangle> rects(kNumChars);
	int rect_area_sum = 0;

	for (unsigned char c = kFirstChar; c < kFirstChar + kNumChars; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
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

	//int atlas_width = 1 << (int)std::ceil(std::log2(std::sqrt(rect_area_sum))); // Round up to nearest power of 2
	int atlas_width = (int)std::ceil(std::sqrt(rect_area_sum));
	while (!rect_pack::RowPacking(atlas_width, atlas_width, rects)) {
		atlas_width = (int)(atlas_width * 1.2f);
	}
	int atlas_height = atlas_width;

	float ratio = (float)rect_area_sum / (atlas_width * atlas_height);
	std::cout << "[Font atlas] res = " << atlas_width << "x" << atlas_height << ", ratio = " << ratio << std::endl;

	// TODO: Zero-initialize texture atlas
	atlas_ = std::make_unique<Texture>(atlas_width, atlas_height, 1, nullptr);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (const auto& rect : rects) {
		const unsigned char c = rect.id;
		const FT_BitmapGlyph& bit_glyph = (FT_BitmapGlyph)glyphs[c - kFirstChar];

		// TODO: Create whole array on the CPU
		atlas_->SubImage(rect.x, rect.y, rect.w, rect.h, bit_glyph->bitmap.buffer);

		const glm::vec2 atlas_size(atlas_width, atlas_height);
		characters_.insert({ c, {
			glm::vec2(rect.x, rect.y) / atlas_size,
			glm::vec2(rect.x + rect.w, rect.y + rect.h) / atlas_size,
			glm::ivec2(rect.w, rect.h),
			glm::ivec2(bit_glyph->left, bit_glyph->top),
			(unsigned int)bit_glyph->root.advance.x >> 16
		} });

		FT_Done_Glyph(glyphs[c - kFirstChar]);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

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
