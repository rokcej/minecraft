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
	void SetColor(const glm::vec3& color);

private:
	void UpdateVertices();

private:
	std::string text_;
	Font* font_;

	glm::vec2 pos_ = { 0.0f, 0.0f };
	glm::vec3 color_ = { 1.0f, 1.0f, 1.0f };

	GLuint vao_;
	GLuint vbo_;

};
