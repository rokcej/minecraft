#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Font;

class Text {
public:
	Text(const std::string& text, Font* font, GLuint shader);
	~Text();

	void SetText(const std::string& text);
	void Render(float x, float y, float scale, glm::vec3 color) const;

private:
	std::string text_;
	Font* font_;
	GLuint shader_;

	GLuint vao_;
	GLuint vbo_;
};
