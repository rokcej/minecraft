#pragma once

#include <string>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;
class Font;

class Text {
public:
	Text(const std::string& text, Font* font);
	~Text();

	void SetText(const std::string& text);
	void Render(const std::unique_ptr<Shader>& shader) const;

	int GetHeight() const;
	void SetPosition(const glm::vec2& pos);
	void SetColor(const glm::vec4& color);
	void SetShadow(int distance, const glm::vec4& color);

private:
	void UpdateVertices();

private:
	std::string text_;
	Font* font_;

	glm::vec2 pos_ = { 0.0f, 0.0f };
	glm::vec4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool shadow_enabled_ = false;
	glm::vec2 shadow_offset_;
	glm::vec4 shadow_color_;

	GLuint vao_;
	GLuint vbo_;

};
