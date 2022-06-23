#include "font.h"

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H 
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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "[ERROR] Failed to load Glyph " << c << std::endl;
			continue;
		}
		characters_[c] = FontCharacter(
			std::make_unique<Texture>(
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				1,
				face->glyph->bitmap.buffer
				),
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

Font::~Font() {

}


FontCharacter::FontCharacter(std::unique_ptr<Texture> texture, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance) { // : texture_(std::move(texture)) {
	texture_ = std::move(texture);
	size_ = size;
	bearing_ = bearing;
	advance_ = advance;
}
