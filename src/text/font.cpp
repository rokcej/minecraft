#include "font.h"

#include <iostream>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <glad/glad.h>
#include <src/gl/texture.h>

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

	int w = 0;
	int h = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 32; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}

		w += face->glyph->bitmap.width;
		h = std::max(h, (int)face->glyph->bitmap.rows);

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

	// TODO: Zero-initialize texture atlas
	atlas_ = std::make_unique<Texture>(w, h, 1, nullptr);

	int x = 0;
	for (int c = 32; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cerr << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		atlas_->SubImage(x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

		characters_[c].uv_ = glm::vec2((float)x / w, 0.0f);
		characters_[c].size_ = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		characters_[c].bearing_ = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		characters_[c].advance_ = face->glyph->advance.x >> 6;

		x += face->glyph->bitmap.width;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

Font::~Font() {

}


FontCharacter::FontCharacter(glm::vec2 uv, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance) {
	uv_ = uv;
	size_ = size;
	bearing_ = bearing;
	advance_ = advance;
}
