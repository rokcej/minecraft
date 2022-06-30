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

	std::vector<rect_pack::Rectangle> rects(128 - 32);
	int rect_area_sum = 0;

	for (unsigned char c = 32; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}

		int width = (int)face->glyph->bitmap.width;
		int height = (int)face->glyph->bitmap.rows;

		rects[c - 32] = { c, width, height, 0, 0 };
		rect_area_sum += width * height;

		/*FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph)) {
			std::cerr << "[ERROR] Failed to get Glyph " << c << std::endl;
			continue;
		}
		if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1)) {
			std::cerr << "[ERROR] Failed to render Glyph" << std::endl;
			continue;
		}
		FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph;
		FT_Done_Glyph(glyph);*/
	}

	//int atlas_width = 1 << (int)std::ceil(std::log2(std::sqrt(rect_area_sum))); // Round up to nearest power of 2
	int atlas_width = (int)std::ceil(std::sqrt(rect_area_sum));
	while (!rect_pack::RowPacking(atlas_width, atlas_width, rects)) {
		//atlas_width *= 2;
		atlas_width = (int)(atlas_width * 1.2f);
	}
	int atlas_height = atlas_width;

	float ratio = (float)rect_area_sum / (atlas_width * atlas_height);
	std::cout << "[Font atlas] res = " << atlas_width << "x" << atlas_height << ", ratio = " << ratio << std::endl;

	// TODO: Zero-initialize texture atlas
	atlas_ = std::make_unique<Texture>(atlas_width, atlas_height, 1, nullptr);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (const auto& rect : rects) {
		const int c = rect.id;

		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		atlas_->SubImage(rect.x, rect.y, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

		characters_[c].uv_min_ = glm::vec2((float)rect.x / atlas_width, (float)rect.y / atlas_height);
		characters_[c].uv_max_ = glm::vec2((float)(rect.x + face->glyph->bitmap.width) / atlas_width, (float)(rect.y + face->glyph->bitmap.rows) / atlas_height);
		characters_[c].size_ = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		characters_[c].bearing_ = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		characters_[c].advance_ = face->glyph->advance.x >> 6;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

Font::~Font() {

}


FontCharacter::FontCharacter(glm::vec2 uv_min, glm::vec2 uv_max, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance) {
	uv_min_ = uv_min;
	uv_max_ = uv_max;
	size_ = size;
	bearing_ = bearing;
	advance_ = advance;
}
