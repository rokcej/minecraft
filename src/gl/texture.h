#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
	Texture(const std::string& file_path);
	~Texture();

	void Bind() const;

private:
	GLuint id_;

	static const int kFallbackWidth;
	static const int kFallbackHeight;
	static const GLubyte kFallbackData[];
};
