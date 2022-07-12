#include "text.h"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <src/text/font.h>
#include <src/gl/texture.h>
#include <src/gl/shader.h>

struct TextVertex {
	GLfloat x;
	GLfloat y;
	GLfloat u;
	GLfloat v;
};

Text::Text(const std::string& text, int size, Font* font) {
	text_ = text;
	size_ = size;
	font_ = font;

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	UpdateVertices();

	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
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
	if (text == text_) {
		return;
	}
	text_ = text;
	UpdateVertices();
}

void Text::UpdateVertices() {
	std::vector<TextVertex> vertices(6 * text_.length());
	int i = 0;
	float advance = 0.0f;
	float y_offset = 0.0f;

	for (const unsigned char& c : text_) {
		if (c == '\n') {
			advance = 0;
			y_offset -= 1.0f;
			continue;
		}
		const FontCharacter& ch = font_->GetCharacter(c);

		glm::vec2 pos0(advance + ch.bearing_.x, y_offset + ch.bearing_.y - ch.size_.y);
		glm::vec2 pos1 = pos0 + ch.size_;

		const glm::vec2& uv0 = ch.uv_min_;
		const glm::vec2& uv1 = ch.uv_max_;

		vertices[i++] = { pos0.x, pos1.y, uv0.x, uv0.y };
		vertices[i++] = { pos0.x, pos0.y, uv0.x, uv1.y };
		vertices[i++] = { pos1.x, pos0.y, uv1.x, uv1.y };
		vertices[i++] = { pos0.x, pos1.y, uv0.x, uv0.y };
		vertices[i++] = { pos1.x, pos0.y, uv1.x, uv1.y };
		vertices[i++] = { pos1.x, pos1.y, uv1.x, uv0.y };

		// Move position to next glyph
		advance += ch.advance_;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Text::Render(const std::unique_ptr<Shader>& shader) const {
	font_->GetAtlas()->Bind();
	glBindVertexArray(vao_);

	shader->SetFloat("uSize", (float)size_);

	if (shadow_enabled_) {
		shader->SetVector2("uPos", pos_ + shadow_offset_);
		shader->SetVector4("uColor", shadow_color_);
		glDrawArrays(GL_TRIANGLES, 0, 6 * (GLsizei)text_.length());
	}

	shader->SetVector2("uPos", pos_);
	shader->SetVector4("uColor", color_);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (GLsizei)text_.length());
}

int Text::GetSize() const {
	return size_;
}

void Text::SetPosition(const glm::vec2& pos) {
	pos_ = pos;
}

void Text::SetColor(const glm::vec4& color) {
	color_ = color;
}

void Text::SetShadow(int distance, const glm::vec4& color) {
	shadow_enabled_ = true;
	shadow_offset_ = glm::vec2((float)distance, (float)-distance);
	shadow_color_ = color;
}
