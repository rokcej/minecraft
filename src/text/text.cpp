#include "text.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/text/font.h>
#include <src/gl/texture.h>


Text::Text(const std::string& text, Font* font, GLuint shader) {
	text_ = text;
	font_ = font;
	shader_ = shader;

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Text::~Text() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
}

void Text::SetText(const std::string& text) {
	text_ = text;
}

void Text::Render(float x, float y, float scale, glm::vec3 color) const {
	glUniform3f(glGetUniformLocation(shader_, "uColor"), color.r, color.g, color.b);
	glBindVertexArray(vao_);

	for (const unsigned char& c : text_) {
		const FontCharacter& ch = font_->characters_.at(c);

		float x0 = x + ch.bearing_.x * scale;
		float y0 = y - (ch.size_.y - ch.bearing_.y) * scale;

		float w = ch.size_.x * scale;
		float h = ch.size_.y * scale;

		float vertices[6][4] = {
			{ x0,     y0 + h,   0.0f, 0.0f },
			{ x0,     y0,       0.0f, 1.0f },
			{ x0 + w, y0,       1.0f, 1.0f },
			{ x0,     y0 + h,   0.0f, 0.0f },
			{ x0 + w, y0,       1.0f, 1.0f },
			{ x0 + w, y0 + h,   1.0f, 0.0f }
		};

		ch.texture_->Bind();
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Move position to next glyph (advance is given in 1/64 pixels)
		x += (ch.advance_ >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
